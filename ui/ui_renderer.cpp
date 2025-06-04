#include "ui_renderer.h"
#include <string>
#include <xenos/xe.h>
#include <xenos/xenos.h>
#include <debug.h>
#include <stdio.h>
#include "../gfx/texture_utils.h"

extern struct XenosDevice *xe;
extern "C" void rgl_init();
extern "C" void rgl_draw_start();
extern "C" void rgl_draw_end();
extern "C" void rgl_color(uint32_t color);
extern "C" void rgl_vertex_2f(float x, float y);
extern "C" void rgl_draw_text(float x, float y, const char *text, uint32_t color);

// Внутреннее состояние
static bool initialized = false;

void UIRender_Init() {
    if (initialized) return;
    rgl_init();
    initialized = true;
}

void UIRender_Begin() {
    rgl_draw_start();
}

void UIRender_End() {
    rgl_draw_end();
}

void UIRender_DrawRect(float x, float y, float w, float h, uint32_t color) {
    rgl_color(color);
    // Рисуем два треугольника
    rgl_vertex_2f(x,     y);
    rgl_vertex_2f(x + w, y);
    rgl_vertex_2f(x,     y + h);

    rgl_vertex_2f(x + w, y);
    rgl_vertex_2f(x + w, y + h);
    rgl_vertex_2f(x,     y + h);
}

void UIRender_DrawText(const std::string &text, float x, float y, uint32_t color) {
    rgl_draw_text(x, y, text.c_str(), color);
}

void DrawTexture(uint32_t texId, float x, float y, float w, float h) {
    Texture_Draw(texId, x, y, w, h);
}
