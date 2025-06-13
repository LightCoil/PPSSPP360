#include "xbox360.hpp"
#include "directx.hpp"
#include "directx_defs.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <future>
#include <string>
#include <sstream>
#include <chrono>
#include <functional>
#include <algorithm>
#include <queue>
#include <condition_variable>
#include <thread>

namespace xbox360 {

namespace {
    // Глобальные переменные для хранения состояния
    std::unordered_map<IDirect3DDevice9*, std::unique_ptr<XBOX_IDirect3DDevice9>> device_instances;
    // ВНИМАНИЕ: std::unique_ptr<IDirect3D*> для COM-объектов с абстрактным деструктором может привести к UB. Лучше использовать обертки XBOX_*
    std::unordered_map<IDirect3DTexture9*, std::unique_ptr<XBOX_IDirect3DTexture9>> texture_instances;
    std::unordered_map<IDirect3DVertexBuffer9*, std::unique_ptr<XBOX_IDirect3DVertexBuffer9>> vertex_buffer_instances;
    std::unordered_map<IDirect3DVertexShader9*, std::unique_ptr<XBOX_IDirect3DVertexShader9>> vertex_shader_instances;
    std::unordered_map<IDirect3DPixelShader9*, std::unique_ptr<XBOX_IDirect3DPixelShader9>> pixel_shader_instances;

    // Очередь команд для асинхронного рендеринга
    std::queue<std::function<void()>> render_queue;
    std::mutex render_mutex;
    std::condition_variable render_cv;
    bool render_thread_running = true;

    void RenderThreadFunc() {
        while (render_thread_running) {
            std::function<void()> cmd;
            {
                std::unique_lock<std::mutex> lock(render_mutex);
                render_cv.wait(lock, [] { return !render_queue.empty() || !render_thread_running; });
                if (!render_thread_running) break;
                cmd = std::move(render_queue.front());
                render_queue.pop();
            }
            if (cmd) cmd();
        }
    }
    std::thread render_thread(RenderThreadFunc);
}

// Thread-safe обертка для всех методов устройства
#define THREAD_SAFE_CALL(cmd) \
    { std::lock_guard<std::mutex> lock(render_mutex); cmd; }
#define ASYNC_RENDER_CALL(cmd) \
    { std::lock_guard<std::mutex> lock(render_mutex); render_queue.push([=](){cmd;}); render_cv.notify_one(); }

// Макрос для подавления предупреждений о неиспользуемых параметрах
#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

// Профилировщик производительности
struct PerformanceProfiler {
    struct ProfileData {
        std::chrono::high_resolution_clock::time_point start_time;
        std::chrono::high_resolution_clock::time_point end_time;
        uint64_t draw_calls = 0;
        uint64_t triangles = 0;
        uint64_t vertices = 0;
        uint64_t texture_changes = 0;
        uint64_t shader_changes = 0;
    };

    std::unordered_map<std::string, ProfileData> profiles;
    std::mutex profiler_mutex;

    void BeginProfile(const std::string& name) {
        std::lock_guard<std::mutex> lock(profiler_mutex);
        profiles[name].start_time = std::chrono::high_resolution_clock::now();
    }

    void EndProfile(const std::string& name) {
        std::lock_guard<std::mutex> lock(profiler_mutex);
        profiles[name].end_time = std::chrono::high_resolution_clock::now();
    }

    void IncrementDrawCalls(const std::string& name) {
        std::lock_guard<std::mutex> lock(profiler_mutex);
        profiles[name].draw_calls++;
    }

    void AddTriangles(const std::string& name, uint64_t count) {
        std::lock_guard<std::mutex> lock(profiler_mutex);
        profiles[name].triangles += count;
    }

    std::string GetProfileReport() {
        std::lock_guard<std::mutex> lock(profiler_mutex);
        std::stringstream ss;
        for (const auto& [name, data] : profiles) {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                data.end_time - data.start_time).count();
            ss << "Profile: " << name << "\n"
               << "Duration: " << duration << "ms\n"
               << "Draw Calls: " << data.draw_calls << "\n"
               << "Triangles: " << data.triangles << "\n"
               << "Vertices: " << data.vertices << "\n"
               << "Texture Changes: " << data.texture_changes << "\n"
               << "Shader Changes: " << data.shader_changes << "\n\n";
        }
        return ss.str();
    }
};

static PerformanceProfiler g_profiler;

// Класс для тройной буферизации
struct TripleBuffer {
    TripleBuffer(
        IDirect3DDevice9* device,
        uint32_t width,
        uint32_t height,
        D3DFORMAT format) : m_device(device), m_current_buffer(0) {
        
        D3DSURFACE_DESC desc;
        desc.Width = width;
        desc.Height = height;
        desc.Format = format;
        desc.Type = D3DRTYPE_SURFACE;
        desc.Usage = D3DUSAGE_RENDERTARGET;
        desc.Pool = D3DPOOL_DEFAULT;
        desc.MultiSampleType = D3DMULTISAMPLE_NONE;
        desc.MultiSampleQuality = 0;
        
        for (int i = 0; i < 3; ++i) {
            IDirect3DSurface9* surface = nullptr;
            HRESULT hr = m_device->CreateOffscreenPlainSurface(
                width, height, format, D3DPOOL_DEFAULT, &surface, nullptr);
            if (SUCCEEDED(hr)) {
                m_buffers[i] = surface;
            }
        }
    }
    
    ~TripleBuffer() {
        for (auto& buffer : m_buffers) {
            if (buffer) {
                buffer->Release();
            }
        }
    }
    
    IDirect3DSurface9* GetCurrentBuffer() const {
        return m_buffers[m_current_buffer];
    }
    
    void SwapBuffers() {
        m_current_buffer = (m_current_buffer + 1) % 3;
    }
    
private:
    IDirect3DDevice9* m_device;
    IDirect3DSurface9* m_buffers[3] = { nullptr, nullptr, nullptr };
    uint32_t m_current_buffer;
};

static std::unique_ptr<TripleBuffer> g_triple_buffer;

// Реализация методов XBOX_IDirect3D9
HRESULT XBOX_IDirect3D9::CreateDevice(
    UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow,
    DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters,
    IDirect3DDevice9** ppReturnedDeviceInterface) {
    
    HRESULT hr = m_d3d->CreateDevice(
        Adapter, DeviceType, hFocusWindow, BehaviorFlags,
        pPresentationParameters, ppReturnedDeviceInterface);
    
    if (SUCCEEDED(hr) && ppReturnedDeviceInterface && *ppReturnedDeviceInterface) {
        // Создаем обертку
        auto wrapper = std::make_unique<XBOX_IDirect3DDevice9>(*ppReturnedDeviceInterface);
        device_instances[*ppReturnedDeviceInterface] = std::move(wrapper);
        *ppReturnedDeviceInterface = reinterpret_cast<IDirect3DDevice9*>(wrapper.get());
        
        // Инициализируем тройную буферизацию
        g_triple_buffer = std::make_unique<TripleBuffer>(
            *ppReturnedDeviceInterface,
            pPresentationParameters->BackBufferWidth,
            pPresentationParameters->BackBufferHeight,
            pPresentationParameters->BackBufferFormat);
    }
    
    return hr;
}

// Реализация методов XBOX_IDirect3DDevice9
HRESULT XBOX_IDirect3DDevice9::Present(
    const RECT* pSourceRect, const RECT* pDestRect,
    HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) {
    
    g_profiler.BeginProfile("Present");
    
    ASYNC_RENDER_CALL(
        if (g_triple_buffer) {
            // Копируем текущий буфер в основной
            IDirect3DSurface9* back_buffer = nullptr;
            m_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back_buffer);
            m_device->StretchRect(g_triple_buffer->GetCurrentBuffer(), nullptr, back_buffer, nullptr, D3DTEXF_NONE);
            back_buffer->Release();
            
            // Переключаем буферы
            g_triple_buffer->SwapBuffers();
        }
        
        m_device->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
    );

    g_profiler.EndProfile("Present");
    return S_OK;
}

HRESULT XBOX_IDirect3DDevice9::Clear(
    DWORD Count, const D3DRECT* pRects, DWORD Flags,
    DWORD Color, float Z, DWORD Stencil) {
    ASYNC_RENDER_CALL(
        m_device->Clear(Count, pRects, Flags, Color, Z, Stencil);
    );
    return S_OK;
}

HRESULT XBOX_IDirect3DDevice9::SetViewport(const D3DVIEWPORT9* pViewport) {
    return m_device->SetViewport(pViewport);
}

HRESULT XBOX_IDirect3DDevice9::BeginScene() {
    return m_device->BeginScene();
}

HRESULT XBOX_IDirect3DDevice9::EndScene() {
    return m_device->EndScene();
}

HRESULT XBOX_IDirect3DDevice9::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) {
    HRESULT hr = m_device->Reset(pPresentationParameters);
    
    if (SUCCEEDED(hr) && pPresentationParameters) {
        g_triple_buffer = std::make_unique<TripleBuffer>(
            m_device,
            pPresentationParameters->BackBufferWidth,
            pPresentationParameters->BackBufferHeight,
            pPresentationParameters->BackBufferFormat
        );
    }
    
    return hr;
}

// Метод для получения отчета о производительности
std::string XBOX_IDirect3DDevice9::GetPerformanceReport() {
    return g_profiler.GetProfileReport();
}

// Реализация методов XBOX_IDirect3DTexture9
XBOX_IDirect3DTexture9::XBOX_IDirect3DTexture9(IDirect3DTexture9* texture) : m_texture(texture) {
    if (m_texture) {
        m_texture->AddRef();
    }
}

XBOX_IDirect3DTexture9::~XBOX_IDirect3DTexture9() {
    if (m_texture) {
        m_texture->Release();
        m_texture = nullptr;
    }
}

HRESULT XBOX_IDirect3DTexture9::GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc) {
    if (!m_texture || !pDesc) {
        return D3DERR_INVALIDCALL;
    }
    return m_texture->GetLevelDesc(Level, pDesc);
}

HRESULT XBOX_IDirect3DTexture9::GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel) {
    if (!m_texture || !ppSurfaceLevel) {
        return D3DERR_INVALIDCALL;
    }
    return m_texture->GetSurfaceLevel(Level, ppSurfaceLevel);
}

HRESULT XBOX_IDirect3DTexture9::LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) {
    if (!m_texture || !pLockedRect) {
        return D3DERR_INVALIDCALL;
    }
    return m_texture->LockRect(Level, pLockedRect, pRect, Flags);
}

HRESULT XBOX_IDirect3DTexture9::UnlockRect(UINT Level) {
    if (!m_texture) {
        return D3DERR_INVALIDCALL;
    }
    return m_texture->UnlockRect(Level);
}

HRESULT XBOX_IDirect3DTexture9::AddDirtyRect(const RECT* pDirtyRect) {
    if (!m_texture) {
        return D3DERR_INVALIDCALL;
    }
    return m_texture->AddDirtyRect(pDirtyRect);
}

HRESULT XBOX_IDirect3DTexture9::GetDevice(XBOX_IDirect3DDevice9** ppDevice) {
    if (!m_texture || !ppDevice) {
        return D3DERR_INVALIDCALL;
    }
    IDirect3DDevice9* device = nullptr;
    HRESULT hr = m_texture->GetDevice(&device);
    if (SUCCEEDED(hr) && device) {
        auto it = device_instances.find(device);
        if (it != device_instances.end()) {
            *ppDevice = reinterpret_cast<XBOX_IDirect3DDevice9*>(it->second.get());
        } else {
            device->Release();
            hr = D3DERR_NOTFOUND;
        }
    }
    return hr;
}

DWORD XBOX_IDirect3DTexture9::SetPriority(DWORD PriorityNew) {
    if (!m_texture) {
        return 0;
    }
    return m_texture->SetPriority(PriorityNew);
}

DWORD XBOX_IDirect3DTexture9::GetPriority() {
    if (!m_texture) {
        return 0;
    }
    return m_texture->GetPriority();
}

void XBOX_IDirect3DTexture9::PreLoad() {
    if (m_texture) {
        m_texture->PreLoad();
    }
}

D3DRESOURCETYPE XBOX_IDirect3DTexture9::GetType() {
    if (!m_texture) {
        return D3DRTYPE_TEXTURE;
    }
    return m_texture->GetType();
}

DWORD XBOX_IDirect3DTexture9::SetLOD(DWORD LODNew) {
    if (!m_texture) {
        return 0;
    }
    return m_texture->SetLOD(LODNew);
}

DWORD XBOX_IDirect3DTexture9::GetLOD() {
    if (!m_texture) {
        return 0;
    }
    return m_texture->GetLOD();
}

DWORD XBOX_IDirect3DTexture9::GetLevelCount() {
    if (!m_texture) {
        return 0;
    }
    return m_texture->GetLevelCount();
}

HRESULT XBOX_IDirect3DTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType) {
    if (!m_texture) {
        return D3DERR_INVALIDCALL;
    }
    return m_texture->SetAutoGenFilterType(FilterType);
}

D3DTEXTUREFILTERTYPE XBOX_IDirect3DTexture9::GetAutoGenFilterType() {
    if (!m_texture) {
        return D3DTEXF_NONE;
    }
    return m_texture->GetAutoGenFilterType();
}

void XBOX_IDirect3DTexture9::GenerateMipSubLevels() {
    if (m_texture) {
        m_texture->GenerateMipSubLevels();
    }
}

// Реализация методов XBOX_IDirect3DVertexShader9
XBOX_IDirect3DVertexShader9::XBOX_IDirect3DVertexShader9(IDirect3DVertexShader9* shader) : m_shader(shader), m_constant_count(0) {
    if (m_shader) {
        m_shader->AddRef();
    }
}

XBOX_IDirect3DVertexShader9::~XBOX_IDirect3DVertexShader9() {
    if (m_shader) {
        m_shader->Release();
        m_shader = nullptr;
    }
}

HRESULT XBOX_IDirect3DVertexShader9::SetConstants(uint32_t start_register, const f32* constant_data, uint32_t vector4f_count) {
    if (!constant_data || vector4f_count == 0) {
        return D3DERR_INVALIDCALL;
    }
    
    size_t required_size = start_register + vector4f_count * 4;
    if (m_constants.size() < required_size) {
        m_constants.resize(required_size);
    }
    
    memcpy(&m_constants[start_register * 4], constant_data, vector4f_count * 4 * sizeof(f32));
    m_constant_count = std::max(m_constant_count, start_register + vector4f_count);
    
    return S_OK;
}

HRESULT XBOX_IDirect3DVertexShader9::GetConstants(uint32_t start_register, f32* constant_data, uint32_t vector4f_count) const {
    if (!constant_data || vector4f_count == 0 || start_register + vector4f_count > m_constant_count) {
        return D3DERR_INVALIDCALL;
    }
    
    memcpy(constant_data, &m_constants[start_register * 4], vector4f_count * 4 * sizeof(f32));
    return S_OK;
}

HRESULT XBOX_IDirect3DVertexShader9::GetDevice(IDirect3DDevice9** ppDevice) {
    if (!m_shader || !ppDevice) {
        return D3DERR_INVALIDCALL;
    }
    return m_shader->GetDevice(ppDevice);
}

HRESULT XBOX_IDirect3DVertexShader9::GetFunction(void* pData, UINT* pSizeOfData) {
    if (!m_shader || !pData || !pSizeOfData) {
        return D3DERR_INVALIDCALL;
    }
    return m_shader->GetFunction(pData, pSizeOfData);
}

// Реализация методов XBOX_IDirect3DPixelShader9
XBOX_IDirect3DPixelShader9::XBOX_IDirect3DPixelShader9(IDirect3DPixelShader9* shader) : m_shader(shader), m_constant_count(0) {
    if (m_shader) {
        m_shader->AddRef();
    }
}

XBOX_IDirect3DPixelShader9::~XBOX_IDirect3DPixelShader9() {
    if (m_shader) {
        m_shader->Release();
        m_shader = nullptr;
    }
}

HRESULT XBOX_IDirect3DPixelShader9::SetConstants(uint32_t start_register, const f32* constant_data, uint32_t vector4f_count) {
    if (!constant_data || vector4f_count == 0) {
        return D3DERR_INVALIDCALL;
    }
    
    size_t required_size = start_register + vector4f_count * 4;
    if (m_constants.size() < required_size) {
        m_constants.resize(required_size);
    }
    
    memcpy(&m_constants[start_register * 4], constant_data, vector4f_count * 4 * sizeof(f32));
    m_constant_count = std::max(m_constant_count, start_register + vector4f_count);
    
    return S_OK;
}

HRESULT XBOX_IDirect3DPixelShader9::GetConstants(uint32_t start_register, f32* constant_data, uint32_t vector4f_count) const {
    if (!constant_data || vector4f_count == 0 || start_register + vector4f_count > m_constant_count) {
        return D3DERR_INVALIDCALL;
    }
    
    memcpy(constant_data, &m_constants[start_register * 4], vector4f_count * 4 * sizeof(f32));
    return S_OK;
}

HRESULT XBOX_IDirect3DPixelShader9::GetDevice(IDirect3DDevice9** ppDevice) {
    if (!m_shader || !ppDevice) {
        return D3DERR_INVALIDCALL;
    }
    return m_shader->GetDevice(ppDevice);
}

HRESULT XBOX_IDirect3DPixelShader9::GetFunction(void* pData, UINT* pSizeOfData) {
    if (!m_shader || !pData || !pSizeOfData) {
        return D3DERR_INVALIDCALL;
    }
    return m_shader->GetFunction(pData, pSizeOfData);
}

// Реализация TextureOptimizer
TextureOptimizer::TextureOptimizer(XBOX_IDirect3DDevice9* device) : m_device(device), m_optimizing(false) {
    ResetStats();
}

TextureOptimizer::~TextureOptimizer() {
    WaitForTextureLoading();
}

HRESULT TextureOptimizer::CreateTexture(
    const std::string& path,
    D3DFORMAT format,
    bool compress,
    XBOX_IDirect3DTexture9** texture) {
    UNUSED(path);
    UNUSED(format);
    UNUSED(compress);
    UNUSED(texture);
    // Реализация создания текстуры
    return E_NOTIMPL;
}

void TextureOptimizer::PreloadTextures(
    const std::vector<std::pair<std::string, D3DFORMAT>>& textures,
    bool compress) {
    UNUSED(textures);
    UNUSED(compress);
    // Реализация предварительной загрузки текстур
}

void TextureOptimizer::WaitForTextureLoading() {
    // Реализация ожидания загрузки текстур
}

void TextureOptimizer::StartOptimization() {
    m_optimizing = true;
}

void TextureOptimizer::StopOptimization() {
    m_optimizing = false;
}

void TextureOptimizer::ResetStats() {
    m_stats = TextureStats();
}

TextureOptimizer::TextureStats TextureOptimizer::GetStats() {
    return m_stats;
}

// Реализация ShaderOptimizer
ShaderOptimizer::ShaderOptimizer(XBOX_IDirect3DDevice9* device) : m_device(device), m_max_cache_size(100) {
    // Инициализация
}

ShaderOptimizer::~ShaderOptimizer() {
    WaitForCompilation();
}

HRESULT ShaderOptimizer::CreateVertexShader(
    const std::string& source,
    const std::string& entry_point,
    XBOX_IDirect3DVertexShader9** shader) {
    UNUSED(source);
    UNUSED(entry_point);
    UNUSED(shader);
    // Реализация создания вершинного шейдера
    return E_NOTIMPL;
}

HRESULT ShaderOptimizer::CreatePixelShader(
    const std::string& source,
    const std::string& entry_point,
    XBOX_IDirect3DPixelShader9** shader) {
    UNUSED(source);
    UNUSED(entry_point);
    UNUSED(shader);
    // Реализация создания пиксельного шейдера
    return E_NOTIMPL;
}

void ShaderOptimizer::PrecompileShaders(
    const std::vector<std::pair<std::string, std::string>>& vertex_shaders,
    const std::vector<std::pair<std::string, std::string>>& pixel_shaders) {
    UNUSED(vertex_shaders);
    UNUSED(pixel_shaders);
    // Реализация предварительной компиляции шейдеров
}

void ShaderOptimizer::WaitForCompilation() {
    // Реализация ожидания компиляции
}

std::future<HRESULT> ShaderOptimizer::CreateVertexShaderAsync(
    const std::vector<uint8_t>& bytecode,
    std::function<void(XBOX_IDirect3DVertexShader9*)> callback) {
    UNUSED(bytecode);
    UNUSED(callback);
    // Реализация асинхронного создания вершинного шейдера
    return std::future<HRESULT>();
}

std::future<HRESULT> ShaderOptimizer::CreatePixelShaderAsync(
    const std::vector<uint8_t>& bytecode,
    std::function<void(XBOX_IDirect3DPixelShader9*)> callback) {
    UNUSED(bytecode);
    UNUSED(callback);
    // Реализация асинхронного создания пиксельного шейдера
    return std::future<HRESULT>();
}

void ShaderOptimizer::SetCacheSize(size_t max_size) {
    m_max_cache_size = max_size;
}

size_t ShaderOptimizer::GetCacheSize() const {
    return m_max_cache_size;
}

void ShaderOptimizer::ClearCache() {
    m_vertex_cache.clear();
    m_pixel_cache.clear();
}

ShaderOptimizer::ShaderStats ShaderOptimizer::GetStats() const {
    return m_stats;
}

// Direct3D functions
XBOX_IDirect3D9* Direct3DCreate9(UINT SDKVersion) {
    IDirect3D9* d3d = ::Direct3DCreate9(SDKVersion);
    if (!d3d) return nullptr;
    return new XBOX_IDirect3D9(d3d);
}

void D3DRelease(XBOX_IDirect3D9* pD3D) {
    if (pD3D) {
        delete pD3D;
    }
}

XBOX_IDirect3DDevice9* Direct3DDevice(XBOX_IDirect3D9* pD3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters) {
    IDirect3DDevice9* device = nullptr;
    HRESULT hr = pD3D->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, &device);
    if (FAILED(hr)) return nullptr;
    return new XBOX_IDirect3DDevice9(device);
}

void D3DReleaseDevice(XBOX_IDirect3DDevice9* pDevice) {
    if (pDevice) {
        delete pDevice;
    }
}

HRESULT D3DBeginScene(XBOX_IDirect3DDevice9* device) {
    return device ? device->BeginScene() : E_FAIL;
}

HRESULT D3DEndScene(XBOX_IDirect3DDevice9* device) {
    return device ? device->EndScene() : E_FAIL;
}

HRESULT D3DPresent(XBOX_IDirect3DDevice9* device, const RECT* src, const RECT* dst, HWND wnd, const RGNDATA* dirty) {
    return device ? device->Present(src, dst, wnd, dirty) : E_FAIL;
}

HRESULT D3DClear(XBOX_IDirect3DDevice9* device, DWORD Count, const D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) {
    return device ? device->Clear(Count, pRects, Flags, Color, Z, Stencil) : E_FAIL;
}

HRESULT D3DSetViewport(XBOX_IDirect3DDevice9* device, const XBOX_D3DVIEWPORT9* viewport) {
    if (!device || !viewport) return E_FAIL;
    D3DVIEWPORT9 vp = {};
    vp.X = viewport->X;
    vp.Y = viewport->Y;
    vp.Width = viewport->Width;
    vp.Height = viewport->Height;
    vp.MinZ = viewport->MinZ;
    vp.MaxZ = viewport->MaxZ;
    return device->SetViewport(&vp);
}

void ConvertPresentParams(const XBOX_D3DPRESENT_PARAMETERS& src, D3DPRESENT_PARAMETERS& dst) {
    memset(&dst, 0, sizeof(dst));
    dst.BackBufferWidth = src.BackBufferWidth;
    dst.BackBufferHeight = src.BackBufferHeight;
    dst.BackBufferFormat = static_cast<D3DFORMAT>(src.BackBufferFormat);
    dst.BackBufferCount = src.BackBufferCount;
    dst.MultiSampleType = static_cast<D3DMULTISAMPLE_TYPE>(src.MultiSampleType);
    dst.MultiSampleQuality = src.MultiSampleQuality;
    dst.SwapEffect = static_cast<D3DSWAPEFFECT>(src.SwapEffect);
    dst.hDeviceWindow = static_cast<HWND>(src.hDeviceWindow);
    dst.Windowed = src.Windowed;
    dst.EnableAutoDepthStencil = src.EnableAutoDepthStencil;
    dst.AutoDepthStencilFormat = static_cast<D3DFORMAT>(src.AutoDepthStencilFormat);
    dst.Flags = src.Flags;
    dst.FullScreen_RefreshRateInHz = src.FullScreen_RefreshRateInHz;
    dst.PresentationInterval = src.PresentationInterval;
}

} // namespace xbox360
