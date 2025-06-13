#pragma once

#include "directx_defs.hpp"
#include <d3d9.h>
#include <d3d9types.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <future>
#include <chrono>
#include <functional>
#include <algorithm>

namespace xbox360 {

class XBOX_IDirect3DDevice9;
struct XBOX_D3DVIEWPORT9;
struct XBOX_D3DPRESENT_PARAMETERS;

// Forward declarations
class XBOX_IDirect3DTexture9;
class XBOX_IDirect3DVertexBuffer9;
class XBOX_IDirect3DIndexBuffer9;
class XBOX_IDirect3DVertexShader9;
class XBOX_IDirect3DPixelShader9;
class XBOX_IDirect3DVertexDeclaration9;
class XBOX_IDirect3DQuery9;

// === BEGIN: Direct3D интерфейсы из directx_base.hpp ===
class Direct3D {
public:
    virtual ~Direct3D() = default;
    virtual XBOX_IDirect3DDevice9* CreateDevice(u32 Adapter, u32 DeviceType, void* hFocusWindow,
        u32 BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters) = 0;
};
class Direct3DDevice {
public:
    virtual ~Direct3DDevice() = default;
    virtual u32 Present(const D3DRECT* pSourceRect, const D3DRECT* pDestRect,
        void* hDestWindowOverride, const void* pDirtyRegion) = 0;
    virtual u32 Clear(u32 Count, const D3DRECT* pRects, u32 Flags,
        u32 Color, f32 Z, u32 Stencil) = 0;
    virtual u32 SetViewport(const D3DVIEWPORT9* pViewport) = 0;
    virtual u32 BeginScene() = 0;
    virtual u32 EndScene() = 0;
    virtual u32 SetStreamSource(u32 StreamNumber, XBOX_IDirect3DVertexBuffer9* pStreamData,
        u32 OffsetInBytes, u32 Stride) = 0;
    virtual u32 SetFVF(u32 FVF) = 0;
    virtual u32 DrawPrimitive(u32 PrimitiveType, u32 StartVertex,
        u32 PrimitiveCount) = 0;
    virtual u32 CreateTexture(u32 Width, u32 Height, u32 Levels, u32 Usage,
        u32 Format, u32 Pool, XBOX_IDirect3DTexture9** ppTexture,
        void* pSharedHandle) = 0;
    virtual u32 CreateVertexBuffer(u32 Length, u32 Usage, u32 FVF,
        u32 Pool, XBOX_IDirect3DVertexBuffer9** ppVertexBuffer,
        void* pSharedHandle) = 0;
    virtual u32 CreateVertexShader(const u32* pFunction,
        XBOX_IDirect3DVertexShader9** ppShader) = 0;
    virtual u32 CreatePixelShader(const u32* pFunction,
        XBOX_IDirect3DPixelShader9** ppShader) = 0;
    virtual u32 SetVertexShader(XBOX_IDirect3DVertexShader9* pShader) = 0;
    virtual u32 SetPixelShader(XBOX_IDirect3DPixelShader9* pShader) = 0;
};
class Direct3DTexture {
public:
    virtual ~Direct3DTexture() = default;
    virtual u32 LockRect(u32 Level, D3DLOCKED_RECT* pLockedRect,
        const D3DRECT* pRect, u32 Flags) = 0;
    virtual u32 UnlockRect(u32 Level) = 0;
};
class Direct3DVertexBuffer {
public:
    virtual ~Direct3DVertexBuffer() = default;
    virtual u32 Lock(u32 OffsetToLock, u32 SizeToLock,
        void** ppbData, u32 Flags) = 0;
    virtual u32 Unlock() = 0;
};
class Direct3DVertexShader {
public:
    virtual ~Direct3DVertexShader() = default;
};
class Direct3DPixelShader {
public:
    virtual ~Direct3DPixelShader() = default;
};
// === END: Direct3D интерфейсы ===

/**
 * @brief Класс для работы с Direct3D
 */
class XBOX_IDirect3D9 {
public:
    XBOX_IDirect3D9(IDirect3D9* d3d) : m_d3d(d3d) {}
    ~XBOX_IDirect3D9() { if (m_d3d) m_d3d->Release(); }

    /**
     * @brief Создает устройство Direct3D
     * @param Adapter Номер адаптера
     * @param DeviceType Тип устройства
     * @param hFocusWindow Окно фокуса
     * @param BehaviorFlags Флаги поведения
     * @param pPresentationParameters Параметры представления
     * @return Указатель на созданное устройство
     */
    HRESULT CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow,
        DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters,
        IDirect3DDevice9** ppReturnedDeviceInterface);

    // Метод доступа к внутреннему объекту
    IDirect3D9* GetD3D() const { return m_d3d; }

private:
    IDirect3D9* m_d3d;
};

/**
 * @brief Класс для работы с устройством Direct3D
 */
class XBOX_IDirect3DDevice9 {
public:
    XBOX_IDirect3DDevice9(IDirect3DDevice9* device) : m_device(device) {}
    ~XBOX_IDirect3DDevice9() { if (m_device) m_device->Release(); }

    // Основные методы устройства
    HRESULT Reset(D3DPRESENT_PARAMETERS* pPresentationParameters);
    HRESULT Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
    HRESULT BeginScene();
    HRESULT EndScene();
    HRESULT Clear(DWORD Count, const D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
    HRESULT SetViewport(const D3DVIEWPORT9* pViewport);
    HRESULT GetViewport(D3DVIEWPORT9* pViewport);
    HRESULT SetScissorRect(const RECT* pRect);
    HRESULT GetScissorRect(RECT* pRect);

    // Методы состояний рендеринга
    HRESULT SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
    HRESULT GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue);
    HRESULT SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
    HRESULT GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue);
    HRESULT SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
    HRESULT GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue);

    // Методы текстур и шейдеров
    HRESULT SetTexture(DWORD Stage, XBOX_IDirect3DTexture9* pTexture);
    HRESULT GetTexture(DWORD Stage, XBOX_IDirect3DTexture9** ppTexture);
    HRESULT SetVertexShader(IDirect3DVertexShader9* pShader);
    HRESULT GetVertexShader(IDirect3DVertexShader9** ppShader);
    HRESULT SetPixelShader(IDirect3DPixelShader9* pShader);
    HRESULT GetPixelShader(IDirect3DPixelShader9** ppShader);
    HRESULT SetVertexDeclaration(XBOX_IDirect3DVertexDeclaration9* pDecl);
    HRESULT GetVertexDeclaration(XBOX_IDirect3DVertexDeclaration9** ppDecl);

    // Методы вершинных буферов
    HRESULT SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData,
        UINT OffsetInBytes, UINT Stride);
    HRESULT GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride);
    HRESULT SetIndices(XBOX_IDirect3DIndexBuffer9* pIndexData);
    HRESULT GetIndices(XBOX_IDirect3DIndexBuffer9** ppIndexData);

    // Методы отрисовки
    HRESULT DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex,
        UINT PrimitiveCount);
    HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);
    HRESULT DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
    HRESULT DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, const void* pIndexData, D3DFORMAT IndexDataFormat, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride);

    // Методы создания ресурсов
    HRESULT CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage,
        D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture,
        HANDLE* pSharedHandle);
    HRESULT CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF,
        D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer,
        HANDLE* pSharedHandle);
    HRESULT CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, XBOX_IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle);
    HRESULT CreateVertexShader(const DWORD* pFunction,
        IDirect3DVertexShader9** ppShader);
    HRESULT CreatePixelShader(const DWORD* pFunction,
        IDirect3DPixelShader9** ppShader);

    // Методы шейдерных констант
    HRESULT SetVertexShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount);
    HRESULT GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount);
    HRESULT SetPixelShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount);
    HRESULT GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount);

    HRESULT SetFVF(DWORD FVF);

    // Метод для получения отчета о производительности
    std::string GetPerformanceReport();

private:
    IDirect3DDevice9* m_device;
};

/**
 * @brief Класс для работы с вершинным буфером
 */
class XBOX_IDirect3DVertexBuffer9 {
private:
    IDirect3DVertexBuffer9* m_buffer; // Внутренний указатель на буфер

public:
    XBOX_IDirect3DVertexBuffer9(IDirect3DVertexBuffer9* buffer) : m_buffer(buffer) {}
    ~XBOX_IDirect3DVertexBuffer9() { if (m_buffer) m_buffer->Release(); }

    HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags);
    HRESULT Unlock();

    // Метод доступа к внутреннему объекту
    IDirect3DVertexBuffer9* GetBuffer() const { return m_buffer; }
};

/**
 * @brief Класс для работы с текстурами на Xbox 360
 */
class XBOX_IDirect3DTexture9 {
public:
    /**
     * @brief Конструктор
     * @param texture Указатель на внутреннюю текстуру
     */
    XBOX_IDirect3DTexture9(IDirect3DTexture9* texture);
    ~XBOX_IDirect3DTexture9();

    /**
     * @brief Получает описание уровня текстуры
     * @param Level Уровень текстуры
     * @param pDesc Указатель на структуру описания
     * @return Код результата
     */
    HRESULT GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc);

    /**
     * @brief Получает поверхность уровня текстуры
     * @param Level Уровень текстуры
     * @param ppSurfaceLevel Указатель на указатель поверхности
     * @return Код результата
     */
    HRESULT GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel);

    /**
     * @brief Блокирует прямоугольную область текстуры
     * @param Level Уровень текстуры
     * @param pLockedRect Указатель на структуру заблокированной области
     * @param pRect Указатель на прямоугольник
     * @param Flags Флаги блокировки
     * @return Код результата
     */
    HRESULT LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags);

    /**
     * @brief Разблокирует прямоугольную область текстуры
     * @param Level Уровень текстуры
     * @return Код результата
     */
    HRESULT UnlockRect(UINT Level);

    /**
     * @brief Добавляет грязный прямоугольник
     * @param pDirtyRect Указатель на прямоугольник
     * @return Код результата
     */
    HRESULT AddDirtyRect(const RECT* pDirtyRect);

    /**
     * @brief Получает устройство
     * @param ppDevice Указатель на указатель устройства
     * @return Код результата
     */
    HRESULT GetDevice(XBOX_IDirect3DDevice9** ppDevice);

    /**
     * @brief Устанавливает приоритет
     * @param PriorityNew Новый приоритет
     * @return Старый приоритет
     */
    DWORD SetPriority(DWORD PriorityNew);

    /**
     * @brief Получает приоритет
     * @return Приоритет
     */
    DWORD GetPriority();

    /**
     * @brief Предварительно загружает текстуру
     */
    void PreLoad();

    /**
     * @brief Получает тип ресурса
     * @return Тип ресурса
     */
    D3DRESOURCETYPE GetType();

    /**
     * @brief Устанавливает уровень детализации
     * @param LODNew Новый уровень детализации
     * @return Старый уровень детализации
     */
    DWORD SetLOD(DWORD LODNew);

    /**
     * @brief Получает уровень детализации
     * @return Уровень детализации
     */
    DWORD GetLOD();

    /**
     * @brief Получает количество уровней
     * @return Количество уровней
     */
    DWORD GetLevelCount();

    /**
     * @brief Устанавливает тип фильтра автоматической генерации
     * @param FilterType Тип фильтра
     * @return Код результата
     */
    HRESULT SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType);

    /**
     * @brief Получает тип фильтра автоматической генерации
     * @return Тип фильтра
     */
    D3DTEXTUREFILTERTYPE GetAutoGenFilterType();

    /**
     * @brief Генерирует подуровни мип-карт
     */
    void GenerateMipSubLevels();

private:
    IDirect3DTexture9* m_texture; // Внутренний указатель на текстуру
};

/**
 * @brief Класс для работы с вершинными шейдерами на Xbox 360
 */
class XBOX_IDirect3DVertexShader9 {
private:
    IDirect3DVertexShader9* m_shader; // Внутренний указатель на шейдер
    std::vector<f32> m_constants; // Шейдерные константы
    uint32_t m_constant_count; // Количество констант

public:
    /**
     * @brief Конструктор
     * @param shader Указатель на внутренний шейдер
     */
    XBOX_IDirect3DVertexShader9(IDirect3DVertexShader9* shader);
    ~XBOX_IDirect3DVertexShader9();

    /**
     * @brief Получает внутренний шейдер
     * @return Указатель на внутренний шейдер
     */
    IDirect3DVertexShader9* GetShader() const { return m_shader; }

    /**
     * @brief Устанавливает шейдерные константы
     * @param start_register Начальный регистр
     * @param constant_data Данные констант
     * @param vector4f_count Количество векторов
     * @return Код результата
     */
    HRESULT SetConstants(uint32_t start_register, const f32* constant_data, uint32_t vector4f_count);

    /**
     * @brief Получает шейдерные константы
     * @param start_register Начальный регистр
     * @param constant_data Данные констант
     * @param vector4f_count Количество векторов
     * @return Код результата
     */
    HRESULT GetConstants(uint32_t start_register, f32* constant_data, uint32_t vector4f_count) const;

    /**
     * @brief Получает количество констант
     * @return Количество констант
     */
    HRESULT GetConstantCount() const { return m_constant_count; }

    HRESULT GetDevice(IDirect3DDevice9** ppDevice);
    HRESULT GetFunction(void* pData, UINT* pSizeOfData);
};

/**
 * @brief Класс для работы с пиксельными шейдерами на Xbox 360
 */
class XBOX_IDirect3DPixelShader9 {
private:
    IDirect3DPixelShader9* m_shader; // Внутренний указатель на шейдер
    std::vector<f32> m_constants; // Шейдерные константы
    uint32_t m_constant_count; // Количество констант

public:
    /**
     * @brief Конструктор
     * @param shader Указатель на внутренний шейдер
     */
    XBOX_IDirect3DPixelShader9(IDirect3DPixelShader9* shader);
    ~XBOX_IDirect3DPixelShader9();

    /**
     * @brief Получает внутренний шейдер
     * @return Указатель на внутренний шейдер
     */
    IDirect3DPixelShader9* GetShader() const { return m_shader; }

    /**
     * @brief Устанавливает шейдерные константы
     * @param start_register Начальный регистр
     * @param constant_data Данные констант
     * @param vector4f_count Количество векторов
     * @return Код результата
     */
    HRESULT SetConstants(uint32_t start_register, const f32* constant_data, uint32_t vector4f_count);

    /**
     * @brief Получает шейдерные константы
     * @param start_register Начальный регистр
     * @param constant_data Данные констант
     * @param vector4f_count Количество векторов
     * @return Код результата
     */
    HRESULT GetConstants(uint32_t start_register, f32* constant_data, uint32_t vector4f_count) const;

    /**
     * @brief Получает количество констант
     * @return Количество констант
     */
    HRESULT GetConstantCount() const { return m_constant_count; }

    HRESULT GetDevice(IDirect3DDevice9** ppDevice);
    HRESULT GetFunction(void* pData, UINT* pSizeOfData);
};

/**
 * @brief Класс для оптимизации текстур
 */
class TextureOptimizer {
public:
    struct TextureStats {
        uint32_t totalTextures;
        uint32_t compressedTextures;
        uint32_t uncompressedTextures;
        uint32_t totalMemory;
        uint32_t compressedMemory;
        uint32_t uncompressedMemory;
        f32 compressionRatio;
        f32 averageLoadTime;
        f32 maxLoadTime;
        f32 minLoadTime;
    };

    /**
     * @brief Конструктор
     * @param device Устройство Direct3D
     */
    TextureOptimizer(XBOX_IDirect3DDevice9* device);
    ~TextureOptimizer();

    /**
     * @brief Создает текстуру с оптимизацией
     * @param path Путь к файлу текстуры
     * @param format Формат текстуры
     * @param compress Сжимать ли текстуру
     * @param texture Указатель на указатель текстуры
     * @return Код результата
     */
    HRESULT CreateTexture(
        const std::string& path,
        D3DFORMAT format,
        bool compress,
        XBOX_IDirect3DTexture9** texture);

    /**
     * @brief Предварительно загружает текстуры
     * @param textures Вектор пар (путь, формат)
     * @param compress Сжимать ли текстуры
     */
    void PreloadTextures(
        const std::vector<std::pair<std::string, D3DFORMAT>>& textures,
        bool compress);

    /**
     * @brief Ожидает завершения загрузки
     */
    void WaitForTextureLoading();

    /**
     * @brief Начинает оптимизацию текстур
     */
    void StartOptimization();

    /**
     * @brief Останавливает оптимизацию текстур
     */
    void StopOptimization();

    /**
     * @brief Сбрасывает статистику текстур
     */
    void ResetStats();

    /**
     * @brief Получает статистику текстур
     * @return Статистика текстур
     */
    TextureStats GetStats();

private:
    XBOX_IDirect3DDevice9* m_device;
    std::mutex m_mutex;
    std::vector<std::future<void>> m_futures;
    TextureStats m_stats;
    bool m_optimizing;
};

/**
 * @brief Класс для оптимизации шейдеров
 */
class ShaderOptimizer {
public:
    struct ShaderStats {
        uint32_t totalShaders;
        uint32_t vertexShaders;
        uint32_t pixelShaders;
        uint32_t totalMemory;
        uint32_t cachedShaders;
        f32 averageCompileTime;
        f32 maxCompileTime;
        f32 minCompileTime;
    };

    /**
     * @brief Конструктор
     * @param device Устройство Direct3D
     */
    ShaderOptimizer(XBOX_IDirect3DDevice9* device);
    ~ShaderOptimizer();

    /**
     * @brief Создает вершинный шейдер с оптимизацией
     * @param source Исходный код шейдера
     * @param entry_point Точка входа
     * @param shader Указатель на указатель шейдера
     * @return Код результата
     */
    HRESULT CreateVertexShader(
        const std::string& source,
        const std::string& entry_point,
        XBOX_IDirect3DVertexShader9** shader);

    /**
     * @brief Создает пиксельный шейдер с оптимизацией
     * @param source Исходный код шейдера
     * @param entry_point Точка входа
     * @param shader Указатель на указатель шейдера
     * @return Код результата
     */
    HRESULT CreatePixelShader(
        const std::string& source,
        const std::string& entry_point,
        XBOX_IDirect3DPixelShader9** shader);

    /**
     * @brief Предварительно компилирует шейдеры
     * @param vertex_shaders Вектор пар (исходный код, точка входа) для вершинных шейдеров
     * @param pixel_shaders Вектор пар (исходный код, точка входа) для пиксельных шейдеров
     */
    void PrecompileShaders(
        const std::vector<std::pair<std::string, std::string>>& vertex_shaders,
        const std::vector<std::pair<std::string, std::string>>& pixel_shaders);

    /**
     * @brief Ожидает завершения компиляции
     */
    void WaitForCompilation();

    /**
     * @brief Асинхронно создает вершинный шейдер
     * @param bytecode Байткод шейдера
     * @param callback Функция обратного вызова
     * @return Future с результатом
     */
    std::future<HRESULT> CreateVertexShaderAsync(
        const std::vector<uint8_t>& bytecode,
        std::function<void(XBOX_IDirect3DVertexShader9*)> callback);

    /**
     * @brief Асинхронно создает пиксельный шейдер
     * @param bytecode Байткод шейдера
     * @param callback Функция обратного вызова
     * @return Future с результатом
     */
    std::future<HRESULT> CreatePixelShaderAsync(
        const std::vector<uint8_t>& bytecode,
        std::function<void(XBOX_IDirect3DPixelShader9*)> callback);

    /**
     * @brief Устанавливает размер кэша шейдеров
     * @param max_size Максимальный размер кэша
     */
    void SetCacheSize(size_t max_size);

    /**
     * @brief Получает размер кэша шейдеров
     * @return Размер кэша
     */
    size_t GetCacheSize() const;

    /**
     * @brief Очищает кэш шейдеров
     */
    void ClearCache();

    /**
     * @brief Получает статистику шейдеров
     * @return Статистика шейдеров
     */
    ShaderStats GetStats() const;

private:
    XBOX_IDirect3DDevice9* m_device;
    std::mutex m_mutex;
    std::vector<std::future<void>> m_futures;
    ShaderStats m_stats;
    std::unordered_map<std::string, std::shared_ptr<XBOX_IDirect3DVertexShader9>> m_vertex_cache;
    std::unordered_map<std::string, std::shared_ptr<XBOX_IDirect3DPixelShader9>> m_pixel_cache;
    size_t m_max_cache_size;
};

// Direct3D functions
XBOX_IDirect3D9* Direct3DCreate9(UINT SDKVersion);
void D3DRelease(XBOX_IDirect3D9* pD3D);
XBOX_IDirect3DDevice9* Direct3DDevice(XBOX_IDirect3D9* pD3D, UINT Adapter,
    D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
    D3DPRESENT_PARAMETERS* pPresentationParameters);
void D3DReleaseDevice(XBOX_IDirect3DDevice9* pDevice);

HRESULT D3DBeginScene(XBOX_IDirect3DDevice9* device);
HRESULT D3DEndScene(XBOX_IDirect3DDevice9* device);
HRESULT D3DPresent(XBOX_IDirect3DDevice9* device, const RECT* src, const RECT* dst, HWND wnd, const RGNDATA* dirty);
HRESULT D3DClear(XBOX_IDirect3DDevice9* device, DWORD Count, const D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
HRESULT D3DSetViewport(XBOX_IDirect3DDevice9* device, const XBOX_D3DVIEWPORT9* viewport);

void ConvertPresentParams(const XBOX_D3DPRESENT_PARAMETERS& src, D3DPRESENT_PARAMETERS& dst);

} // namespace xbox360
