// gfx/render.h
#pragma once

#include <stdint.h>
#include <stddef.h>

enum class RenderBackend {
    XENON_GPU,
    SOFTWARE
};

struct FrameBuffer {
    void *colorBuffer;
    void *depthBuffer;
    int width;
    int height;
};

class Renderer {
public:
    virtual ~Renderer() {}

    virtual bool Initialize(int width, int height) = 0;
    virtual void Shutdown() = 0;

    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;

    virtual void Clear(uint32_t color) = 0;
    virtual void DrawTriangleList(const void* vertices, size_t count) = 0;

    virtual FrameBuffer* GetCurrentFrameBuffer() = 0;

    static Renderer* Create(RenderBackend backend);
};