#ifndef VIDEO_HPP
#define VIDEO_HPP

#include "../xbox360/xbox360.hpp"
#include <memory>
#include <windows.h>
#include "../xbox360/directx.hpp"

namespace Xbox360 {
using namespace xbox360;

// Direct3D constants
#define D3D_SDK_VERSION 32
#define D3DDEVTYPE_HAL 1
#define D3DPOOL_DEFAULT 0

class GraphicsDevice {
public:
    GraphicsDevice();
    ~GraphicsDevice();

    bool Initialize(HWND hwnd, const XBOX_D3DPRESENT_PARAMETERS* params);
    void Shutdown();
    bool IsInitialized() const;

    // Основные функции рендеринга
    bool BeginScene();
    bool EndScene();
    bool Present();
    bool Clear(u32 color);
    bool SetViewport(const XBOX_D3DVIEWPORT9* viewport);

    // Работа с текстурами
    bool CreateTexture(u32 width, u32 height, u32 levels, u32 usage, u32 format, XBOX_IDirect3DTexture9** texture);
    bool LockTexture(XBOX_IDirect3DTexture9* texture, u32 level, void** data, u32* pitch);
    bool UnlockTexture(XBOX_IDirect3DTexture9* texture, u32 level);
    void ReleaseTexture(XBOX_IDirect3DTexture9* texture);

    // Работа с вершинными буферами
    bool CreateVertexBuffer(u32 length, u32 usage, u32 fvf, XBOX_IDirect3DVertexBuffer9** buffer);
    bool LockVertexBuffer(XBOX_IDirect3DVertexBuffer9* buffer, u32 offset, u32 size, void** data);
    bool UnlockVertexBuffer(XBOX_IDirect3DVertexBuffer9* buffer);
    void ReleaseVertexBuffer(XBOX_IDirect3DVertexBuffer9* buffer);

    // Работа с шейдерами
    bool CreateVertexShader(const void* function, XBOX_IDirect3DVertexShader9** shader);
    bool CreatePixelShader(const void* function, XBOX_IDirect3DPixelShader9** shader);
    bool SetVertexShader(XBOX_IDirect3DVertexShader9* shader);
    bool SetPixelShader(XBOX_IDirect3DPixelShader9* shader);
    void ReleaseVertexShader(XBOX_IDirect3DVertexShader9* shader);
    void ReleasePixelShader(XBOX_IDirect3DPixelShader9* shader);

    // Рисование примитивов
    bool SetStreamSource(u32 stream, XBOX_IDirect3DVertexBuffer9* buffer, u32 offset, u32 stride);
    bool SetFVF(u32 fvf);
    bool DrawPrimitive(u32 type, u32 start, u32 count);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

class Graphics {
public:
    Graphics();
    ~Graphics();

    bool Initialize(HWND focusWindow, const XBOX_D3DPRESENT_PARAMETERS* params);
    void Shutdown();
    bool IsInitialized() const;

    GraphicsDevice* GetDevice() const { return device; }

private:
    GraphicsDevice* device;
};

} // namespace Xbox360

#endif // VIDEO_HPP 