#include "video.h"
#include <stdexcept>
#include "../xbox360/directx.hpp"
#include <d3d9.h>

namespace ppsspp {
namespace core {

// Вершинный шейдер для отрисовки PSP кадра
const char* VERTEX_SHADER = R"(
    float4x4 WorldViewProj;
    struct VS_INPUT {
        float4 Position : POSITION0;
        float2 TexCoord : TEXCOORD0;
    };
    struct VS_OUTPUT {
        float4 Position : POSITION0;
        float2 TexCoord : TEXCOORD0;
    };
    VS_OUTPUT main(VS_INPUT input) {
        VS_OUTPUT output;
        output.Position = mul(input.Position, WorldViewProj);
        output.TexCoord = input.TexCoord;
        return output;
    }
)";

// Пиксельный шейдер для отрисовки PSP кадра
const char* PIXEL_SHADER = R"(
    sampler2D TextureSampler;
    struct PS_INPUT {
        float2 TexCoord : TEXCOORD0;
    };
    float4 main(PS_INPUT input) : COLOR0 {
        return tex2D(TextureSampler, input.TexCoord);
    }
)";

// Структура вершин для полноэкранного квада
struct Vertex {
    float x, y, z, w;
    float u, v;
};

VideoSystem& VideoSystem::GetInstance() {
    static VideoSystem instance;
    return instance;
}

VideoSystem::VideoSystem() 
    : isInitialized_(false) {
    d3d_ = nullptr;
    device_ = nullptr;
    swapChain_ = nullptr;
    backBuffer_ = nullptr;
    pspFrameBuffer_ = nullptr;
    renderTarget_ = nullptr;
    quadVertexBuffer_ = nullptr;
    pixelShader_ = nullptr;
    vertexShader_ = nullptr;
    
    state_.displayWidth = PSP_WIDTH;
    state_.displayHeight = PSP_HEIGHT;
    state_.vsyncEnabled = true;
    state_.frameSkipEnabled = false;
    state_.isInitialized = false;
    
    frameBuffer_.resize(PSP_FRAMEBUFFER_SIZE);
}

VideoSystem::~VideoSystem() {
    Shutdown();
}

bool VideoSystem::Initialize() {
    if (isInitialized_) {
        return true;
    }

    if (!InitializeD3D()) {
        return false;
    }

    CreateRenderTargets();
    UpdateViewport();

    state_.isInitialized = true;
    isInitialized_ = true;
    return true;
}

void VideoSystem::Shutdown() {
    if (!isInitialized_) {
        return;
    }

    DestroyRenderTargets();

    if (d3d_) {
        delete d3d_;
        d3d_ = nullptr;
    }

    if (device_) {
        delete device_;
        device_ = nullptr;
    }

    if (swapChain_) {
        swapChain_->Release();
        swapChain_ = nullptr;
    }

    if (backBuffer_) {
        backBuffer_->Release();
        backBuffer_ = nullptr;
    }

    if (pspFrameBuffer_) {
        delete pspFrameBuffer_;
        pspFrameBuffer_ = nullptr;
    }

    if (renderTarget_) {
        delete renderTarget_;
        renderTarget_ = nullptr;
    }

    if (quadVertexBuffer_) {
        delete quadVertexBuffer_;
        quadVertexBuffer_ = nullptr;
    }

    if (pixelShader_) {
        delete pixelShader_;
        pixelShader_ = nullptr;
    }

    if (vertexShader_) {
        delete vertexShader_;
        vertexShader_ = nullptr;
    }

    state_.isInitialized = false;
    isInitialized_ = false;
}

bool VideoSystem::InitializeD3D() {
    // Создание Direct3D
    IDirect3D9* d3d_native = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d_native) {
        return false;
    }
    d3d_ = new xbox360::XBOX_IDirect3D9(d3d_native);

    // Инициализация параметров презентации
    D3DPRESENT_PARAMETERS presentParams = {};
    presentParams.BackBufferWidth = state_.displayWidth;
    presentParams.BackBufferHeight = state_.displayHeight;
    presentParams.BackBufferFormat = D3DFMT_A8R8G8B8;
    presentParams.BackBufferCount = 1;
    presentParams.MultiSampleType = D3DMULTISAMPLE_NONE;
    presentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    presentParams.hDeviceWindow = nullptr;
    presentParams.Windowed = FALSE;
    presentParams.EnableAutoDepthStencil = TRUE;
    presentParams.AutoDepthStencilFormat = D3DFMT_D24S8;
    presentParams.PresentationInterval = state_.vsyncEnabled ?
        D3DPRESENT_INTERVAL_ONE :
        D3DPRESENT_INTERVAL_IMMEDIATE;

    IDirect3DDevice9* device_native = nullptr;
    HRESULT hr = d3d_native->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        nullptr,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &presentParams,
        &device_native
    );
    if (FAILED(hr) || !device_native) {
        delete d3d_;
        d3d_ = nullptr;
        return false;
    }
    device_ = new xbox360::XBOX_IDirect3DDevice9(device_native);
    return true;
}

void VideoSystem::CreateRenderTargets() {
    // Создание буфера для PSP кадра
    IDirect3DTexture9* dxTexture = nullptr;
    reinterpret_cast<IDirect3DDevice9*>(device_)->CreateTexture(
        PSP_WIDTH,
        PSP_HEIGHT,
        1,
        D3DUSAGE_RENDERTARGET,
        D3DFMT_A8R8G8B8,
        D3DPOOL_DEFAULT,
        &dxTexture,
        nullptr
    );
    pspFrameBuffer_ = new xbox360::XBOX_IDirect3DTexture9(dxTexture);

    // Создание вершинного буфера для полноэкранного квада
    Vertex vertices[] = {
        {-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f},
        { 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f},
        {-1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f},
        { 1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f}
    };

    IDirect3DVertexBuffer9* dxVB = nullptr;
    reinterpret_cast<IDirect3DDevice9*>(device_)->CreateVertexBuffer(
        sizeof(vertices),
        D3DUSAGE_WRITEONLY,
        D3DFVF_XYZRHW | D3DFVF_TEX1,
        D3DPOOL_DEFAULT,
        &dxVB,
        nullptr
    );
    quadVertexBuffer_ = new xbox360::XBOX_IDirect3DVertexBuffer9(dxVB);

    void* data = nullptr;
    if (quadVertexBuffer_->Lock(0, sizeof(vertices), &data, 0) == S_OK) {
        memcpy(data, vertices, sizeof(vertices));
        quadVertexBuffer_->Unlock();
    }
}

void VideoSystem::DestroyRenderTargets() {
    if (pspFrameBuffer_) {
        delete pspFrameBuffer_;
        pspFrameBuffer_ = nullptr;
    }

    if (quadVertexBuffer_) {
        delete quadVertexBuffer_;
        quadVertexBuffer_ = nullptr;
    }
}

void VideoSystem::UpdateViewport() {
    D3DVIEWPORT9 viewport = {};
    viewport.X = 0;
    viewport.Y = 0;
    viewport.Width = state_.displayWidth;
    viewport.Height = state_.displayHeight;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;

    device_->SetViewport(&viewport);
}

void VideoSystem::SetDisplayParams(uint32_t width, uint32_t height, bool vsync) {
    if (width != state_.displayWidth || height != state_.displayHeight || vsync != state_.vsyncEnabled) {
        state_.displayWidth = width;
        state_.displayHeight = height;
        state_.vsyncEnabled = vsync;

        if (isInitialized_) {
            Shutdown();
            Initialize();
        }
    }
}

void VideoSystem::Render() {
    if (!isInitialized_) {
        return;
    }

    // Очистка буфера
    device_->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF000000, 1.0f, 0);

    // Начало сцены
    if (device_->BeginScene()) {
        // Установка вершинного буфера
        device_->SetStreamSource(0, quadVertexBuffer_->GetBuffer(), 0, sizeof(Vertex));
        device_->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);

        // Отрисовка квада
        device_->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

        // Окончание сцены
        device_->EndScene();
    }

    // Представление кадра
    device_->Present(nullptr, nullptr, nullptr, nullptr);
}

void VideoSystem::ProcessDisplayList(const DisplayList& list) {
    if (!isInitialized_) {
        return;
    }

    // TODO: Обработка команд дисплейного списка PSP
}

void VideoSystem::ClearScreen() {
    if (!isInitialized_ || !device_) {
        return;
    }

    device_->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255, 0, 0, 0), 1.0f, 0);
}

void VideoSystem::WaitVSync() {
    if (!isInitialized_ || !device_) {
        return;
    }

    device_->Present(nullptr, nullptr, nullptr, nullptr);
}

} // namespace core
} // namespace ppsspp
