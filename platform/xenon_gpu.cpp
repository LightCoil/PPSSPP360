// platform/xenon_gpu.cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ppc/timebase.h>
#include <xenos/xe.h>
#include <xenos/xenos.h>
#include <xenos/commands.h>
#include <xenos/texture.h>
#include <xenos/vertices.h>

static struct XenosDevice g_pGPU;
static struct XenosSurface *g_pBackBuffer = nullptr;

extern "C" void xenon_init_gpu() {
    // Инициализация GPU через libxenon
    Xe_Init(&g_pGPU);
    g_pBackBuffer = Xe_GetFramebufferSurface(&g_pGPU);
    Xe_InvalidateState(&g_pGPU);

    printf("[xenon_gpu] GPU инициализирован\n");
}

extern "C" void xenon_swap_buffers() {
    Xe_Resolve(&g_pGPU);
    while (!Xe_IsVBlank(&g_pGPU)) {
        udelay(50); // подождать вертикальной синхронизации
    }
    Xe_SwapBuffers(&g_pGPU);
}

extern "C" void xenon_clear(uint32_t color) {
    // Простая очистка экрана прямоугольником
    Xe_SetClearColor(&g_pGPU, color);
    Xe_Clear(&g_pGPU, XE_CLEAR_COLOR | XE_CLEAR_ZBUFFER);
}

extern "C" void xenon_draw_triangles(const void* vertices, size_t count) {
    if (count == 0) return;

    // Создание буфера вершин
    struct VertexBuffer *vb = Xe_CreateVertexBuffer(&g_pGPU, count * sizeof(float) * 8);
    void* buffer = Xe_VB_Lock(vb);
    memcpy(buffer, vertices, count * sizeof(float) * 8);
    Xe_VB_Unlock(vb);

    // Настройка формата
    Xe_SetVertexFormat(&g_pGPU, XE_VFMT_FLOAT3, XE_VFMT_FLOAT3, XE_VFMT_FLOAT2);
    Xe_SetStreamSource(&g_pGPU, 0, vb, 0, sizeof(float) * 8);

    // Рисование
    Xe_DrawPrimitive(&g_pGPU, XE_PRIMTYPE_TRIANGLELIST, 0, count / 3);

    // Очистка
    Xe_DestroyVertexBuffer(vb);
}