#include <windows.h>
#include "../xbox360/directx.hpp"
#include "video.hpp"
#include <memory>

namespace Xbox360 {

class GraphicsDevice::Impl {
public:
    Impl() : isInitialized(false), d3d(nullptr), device(nullptr) {}

    bool Initialize(HWND hwnd, const XBOX_D3DPRESENT_PARAMETERS* params) {
        if (isInitialized) return true;

        // Создаем Direct3D
        d3d = xbox360::Direct3DCreate9(D3D_SDK_VERSION);
        if (!d3d) return false;

        // Создаем устройство
        D3DPRESENT_PARAMETERS d3dParams;
        ConvertPresentParams(*params, d3dParams);
        device = Direct3DDevice(d3d, 0, static_cast<D3DDEVTYPE>(D3DDEVTYPE_HAL), hwnd,
            D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dParams);
        if (!device) {
            D3DRelease(d3d);
            d3d = nullptr;
            return false;
        }

        isInitialized = true;
        return true;
    }

    void Shutdown() {
        if (!isInitialized) return;

        if (device) {
            D3DReleaseDevice(device);
            device = nullptr;
        }

        if (d3d) {
            D3DRelease(d3d);
            d3d = nullptr;
        }

        isInitialized = false;
    }

    bool IsInitialized() const {
        return isInitialized;
    }

    // Основные функции рендеринга
    bool BeginScene() {
        return SUCCEEDED(D3DBeginScene(device));
    }

    bool EndScene() {
        return SUCCEEDED(D3DEndScene(device));
    }

    bool Present() {
        return SUCCEEDED(D3DPresent(device, nullptr, nullptr, nullptr, nullptr));
    }

    bool Clear(u32 color) {
        return SUCCEEDED(D3DClear(device, 0, nullptr, D3DCLEAR_TARGET, color, 1.0f, 0));
    }

    bool SetViewport(const XBOX_D3DVIEWPORT9* viewport) {
        return SUCCEEDED(D3DSetViewport(device, viewport));
    }

    // Работа с текстурами
    bool CreateTexture(u32 width, u32 height, u32 levels, u32 usage, u32 format, XBOX_IDirect3DTexture9** texture) {
        IDirect3DTexture9* nativeTexture = nullptr;
        HRESULT hr = device->CreateTexture(width, height, levels, usage, static_cast<D3DFORMAT>(format), static_cast<D3DPOOL>(D3DPOOL_DEFAULT), &nativeTexture, nullptr);
        if (SUCCEEDED(hr)) {
            *texture = new XBOX_IDirect3DTexture9(nativeTexture);
        }
        return SUCCEEDED(hr);
    }

    bool LockTexture(XBOX_IDirect3DTexture9* texture, u32 level, void** data, u32* pitch) {
        D3DLOCKED_RECT lockedRect;
        HRESULT hr = texture->LockRect(level, &lockedRect, nullptr, 0);
        if (SUCCEEDED(hr)) {
            *data = lockedRect.pBits;
            *pitch = lockedRect.Pitch;
        }
        return SUCCEEDED(hr);
    }

    bool UnlockTexture(XBOX_IDirect3DTexture9* texture, u32 level) {
        return SUCCEEDED(texture->UnlockRect(level));
    }

    void ReleaseTexture(XBOX_IDirect3DTexture9* texture) {
        delete texture;
    }

    // Работа с вершинными буферами
    bool CreateVertexBuffer(u32 length, u32 usage, u32 fvf, XBOX_IDirect3DVertexBuffer9** buffer) {
        IDirect3DVertexBuffer9* nativeBuffer = nullptr;
        HRESULT hr = device->CreateVertexBuffer(length, usage, fvf, static_cast<D3DPOOL>(D3DPOOL_DEFAULT), &nativeBuffer, nullptr);
        if (SUCCEEDED(hr)) {
            *buffer = new XBOX_IDirect3DVertexBuffer9(nativeBuffer);
        }
        return SUCCEEDED(hr);
    }

    bool LockVertexBuffer(XBOX_IDirect3DVertexBuffer9* buffer, u32 offset, u32 size, void** data) {
        return SUCCEEDED(buffer->Lock(offset, size, data, 0));
    }

    bool UnlockVertexBuffer(XBOX_IDirect3DVertexBuffer9* buffer) {
        return SUCCEEDED(buffer->Unlock());
    }

    void ReleaseVertexBuffer(XBOX_IDirect3DVertexBuffer9* buffer) {
        delete buffer;
    }

    // Работа с шейдерами
    bool CreateVertexShader(const void* function, XBOX_IDirect3DVertexShader9** shader) {
        IDirect3DVertexShader9* nativeShader = nullptr;
        HRESULT hr = device->CreateVertexShader(static_cast<const DWORD*>(function), &nativeShader);
        if (SUCCEEDED(hr)) {
            *shader = new XBOX_IDirect3DVertexShader9(nativeShader);
        }
        return SUCCEEDED(hr);
    }

    bool CreatePixelShader(const void* function, XBOX_IDirect3DPixelShader9** shader) {
        IDirect3DPixelShader9* nativeShader = nullptr;
        HRESULT hr = device->CreatePixelShader(static_cast<const DWORD*>(function), &nativeShader);
        if (SUCCEEDED(hr)) {
            *shader = new XBOX_IDirect3DPixelShader9(nativeShader);
        }
        return SUCCEEDED(hr);
    }

    bool SetVertexShader(XBOX_IDirect3DVertexShader9* shader) {
        return SUCCEEDED(device->SetVertexShader(shader ? shader->GetShader() : nullptr));
    }

    bool SetPixelShader(XBOX_IDirect3DPixelShader9* shader) {
        return SUCCEEDED(device->SetPixelShader(shader ? shader->GetShader() : nullptr));
    }

    void ReleaseVertexShader(XBOX_IDirect3DVertexShader9* shader) {
        delete shader;
    }

    void ReleasePixelShader(XBOX_IDirect3DPixelShader9* shader) {
        delete shader;
    }

    // Рисование примитивов
    bool SetStreamSource(u32 stream, XBOX_IDirect3DVertexBuffer9* buffer, u32 offset, u32 stride) {
        return SUCCEEDED(device->SetStreamSource(stream, buffer ? buffer->GetBuffer() : nullptr, offset, stride));
    }

    bool SetFVF(u32 fvf) {
        return SUCCEEDED(device->SetFVF(fvf));
    }

    bool DrawPrimitive(u32 type, u32 start, u32 count) {
        return SUCCEEDED(device->DrawPrimitive(static_cast<D3DPRIMITIVETYPE>(type), start, count));
    }

private:
    bool isInitialized;
    XBOX_IDirect3D9* d3d;
    XBOX_IDirect3DDevice9* device;
};

// Реализация публичного интерфейса GraphicsDevice
GraphicsDevice::GraphicsDevice() : pImpl(std::make_unique<Impl>()) {}
GraphicsDevice::~GraphicsDevice() = default;

bool GraphicsDevice::Initialize(HWND hwnd, const XBOX_D3DPRESENT_PARAMETERS* params) {
    return pImpl->Initialize(hwnd, params);
}

void GraphicsDevice::Shutdown() {
    pImpl->Shutdown();
}

bool GraphicsDevice::IsInitialized() const {
    return pImpl->IsInitialized();
}

bool GraphicsDevice::BeginScene() {
    return pImpl->BeginScene();
}

bool GraphicsDevice::EndScene() {
    return pImpl->EndScene();
}

bool GraphicsDevice::Present() {
    return pImpl->Present();
}

bool GraphicsDevice::Clear(u32 color) {
    return pImpl->Clear(color);
}

bool GraphicsDevice::SetViewport(const XBOX_D3DVIEWPORT9* viewport) {
    return pImpl->SetViewport(viewport);
}

bool GraphicsDevice::CreateTexture(u32 width, u32 height, u32 levels, u32 usage, u32 format, XBOX_IDirect3DTexture9** texture) {
    return pImpl->CreateTexture(width, height, levels, usage, format, texture);
}

bool GraphicsDevice::LockTexture(XBOX_IDirect3DTexture9* texture, u32 level, void** data, u32* pitch) {
    return pImpl->LockTexture(texture, level, data, pitch);
}

bool GraphicsDevice::UnlockTexture(XBOX_IDirect3DTexture9* texture, u32 level) {
    return pImpl->UnlockTexture(texture, level);
}

void GraphicsDevice::ReleaseTexture(XBOX_IDirect3DTexture9* texture) {
    pImpl->ReleaseTexture(texture);
}

bool GraphicsDevice::CreateVertexBuffer(u32 length, u32 usage, u32 fvf, XBOX_IDirect3DVertexBuffer9** buffer) {
    return pImpl->CreateVertexBuffer(length, usage, fvf, buffer);
}

bool GraphicsDevice::LockVertexBuffer(XBOX_IDirect3DVertexBuffer9* buffer, u32 offset, u32 size, void** data) {
    return pImpl->LockVertexBuffer(buffer, offset, size, data);
}

bool GraphicsDevice::UnlockVertexBuffer(XBOX_IDirect3DVertexBuffer9* buffer) {
    return pImpl->UnlockVertexBuffer(buffer);
}

void GraphicsDevice::ReleaseVertexBuffer(XBOX_IDirect3DVertexBuffer9* buffer) {
    pImpl->ReleaseVertexBuffer(buffer);
}

bool GraphicsDevice::CreateVertexShader(const void* function, XBOX_IDirect3DVertexShader9** shader) {
    return pImpl->CreateVertexShader(function, shader);
}

bool GraphicsDevice::CreatePixelShader(const void* function, XBOX_IDirect3DPixelShader9** shader) {
    return pImpl->CreatePixelShader(function, shader);
}

bool GraphicsDevice::SetVertexShader(XBOX_IDirect3DVertexShader9* shader) {
    return pImpl->SetVertexShader(shader);
}

bool GraphicsDevice::SetPixelShader(XBOX_IDirect3DPixelShader9* shader) {
    return pImpl->SetPixelShader(shader);
}

void GraphicsDevice::ReleaseVertexShader(XBOX_IDirect3DVertexShader9* shader) {
    pImpl->ReleaseVertexShader(shader);
}

void GraphicsDevice::ReleasePixelShader(XBOX_IDirect3DPixelShader9* shader) {
    pImpl->ReleasePixelShader(shader);
}

bool GraphicsDevice::SetStreamSource(u32 stream, XBOX_IDirect3DVertexBuffer9* buffer, u32 offset, u32 stride) {
    return pImpl->SetStreamSource(stream, buffer, offset, stride);
}

bool GraphicsDevice::SetFVF(u32 fvf) {
    return pImpl->SetFVF(fvf);
}

bool GraphicsDevice::DrawPrimitive(u32 type, u32 start, u32 count) {
    return pImpl->DrawPrimitive(type, start, count);
}

// Реализация класса Graphics
Graphics::Graphics() : device(nullptr) {}
Graphics::~Graphics() { Shutdown(); }

bool Graphics::Initialize(HWND focusWindow, const XBOX_D3DPRESENT_PARAMETERS* params) {
    if (device) return true;

    device = new GraphicsDevice();
    return device->Initialize(focusWindow, params);
}

void Graphics::Shutdown() {
    if (device) {
        device->Shutdown();
        delete device;
        device = nullptr;
    }
}

bool Graphics::IsInitialized() const {
    return device && device->IsInitialized();
}

} // namespace Xbox360
