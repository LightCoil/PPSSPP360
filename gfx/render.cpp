// gfx/render.cpp
#include "render.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Простейшие заглушки GX API для Xbox 360
extern "C" {
    void xenon_init_gpu();
    void xenon_swap_buffers();
    void xenon_clear(uint32_t color);
    void xenon_draw_triangles(const void* vertices, size_t count);
}

class XenonRenderer : public Renderer {
public:
    XenonRenderer() : framebuffer{} {}

    bool Initialize(int width, int height) override {
        xenon_init_gpu();
        framebuffer.width = width;
        framebuffer.height = height;
        framebuffer.colorBuffer = malloc(width * height * 4);
        framebuffer.depthBuffer = malloc(width * height * 4);
        return framebuffer.colorBuffer != nullptr && framebuffer.depthBuffer != nullptr;
    }

    void Shutdown() override {
        free(framebuffer.colorBuffer);
        free(framebuffer.depthBuffer);
    }

    void BeginFrame() override {
        // Очистка экрана перед кадром
        xenon_clear(0xFF000000); // чёрный фон
    }

    void EndFrame() override {
        xenon_swap_buffers();
    }

    void Clear(uint32_t color) override {
        xenon_clear(color);
    }

    void DrawTriangleList(const void* vertices, size_t count) override {
        xenon_draw_triangles(vertices, count);
    }

    FrameBuffer* GetCurrentFrameBuffer() override {
        return &framebuffer;
    }

private:
    FrameBuffer framebuffer;
};

Renderer* Renderer::Create(RenderBackend backend) {
    if (backend == RenderBackend::XENON_GPU) {
        return new XenonRenderer();
    }

    // В будущем: добавить software backend
    return nullptr;
}