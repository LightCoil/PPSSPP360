#ifndef VIDEO_H
#define VIDEO_H

#include <cstdint>
#include <vector>
#include "../xbox360/directx.hpp"

namespace ppsspp {
namespace core {

// Константы для размеров PSP
constexpr uint32_t PSP_WIDTH = 480;
constexpr uint32_t PSP_HEIGHT = 272;
constexpr uint32_t PSP_FRAMEBUFFER_SIZE = PSP_WIDTH * PSP_HEIGHT * 4;

struct DisplayList;

class VideoSystem {
public:
    static VideoSystem& GetInstance();

    bool Initialize();
    void Shutdown();
    void Render();
    void SetDisplayParams(uint32_t width, uint32_t height, bool vsync);

    // Восстановленные методы
    void ProcessDisplayList(const DisplayList& list);
    void ClearScreen();
    void WaitVSync();

private:
    VideoSystem();
    ~VideoSystem();

    bool InitializeD3D();
    void CreateRenderTargets();
    void UpdateViewport();
    void DestroyRenderTargets();

    bool isInitialized_;
    xbox360::XBOX_IDirect3D9* d3d_;
    xbox360::XBOX_IDirect3DDevice9* device_;
    xbox360::XBOX_IDirect3DTexture9* pspFrameBuffer_;
    xbox360::XBOX_IDirect3DTexture9* renderTarget_;
    xbox360::XBOX_IDirect3DVertexBuffer9* quadVertexBuffer_;
    xbox360::XBOX_IDirect3DVertexShader9* vertexShader_;
    xbox360::XBOX_IDirect3DPixelShader9* pixelShader_;

    // Добавлено для поддержки COM-объектов
    IDirect3DSwapChain9* swapChain_;
    IDirect3DSurface9* backBuffer_;

    struct {
        uint32_t displayWidth;
        uint32_t displayHeight;
        bool vsyncEnabled;
        bool frameSkipEnabled;
        bool isInitialized;
    } state_;

    std::vector<uint8_t> frameBuffer_;
};

} // namespace core
} // namespace ppsspp

#endif // VIDEO_H
