#include "psp_display.h"
#include <xenos/xe.h>
#include <xenos/xenos.h>

static int screenW = 1280;
static int screenH = 720;

void Display_Init() {
    // Инициализируем Xenos, рендер-таргет и др.
    XenosSurface_Init(screenW, screenH, 4, XENON_SURFACE_FMT_8888);
}

void Display_Shutdown() {
    // Освободить ресурсы, если нужно
}

int Display_GetWidth() {
    return screenW;
}

int Display_GetHeight() {
    return screenH;
}

void Display_ReadPixels(uint8_t *outRGBA) {
    // Копируем пиксели из backbuffer (RGBA)
    xenos_read_backbuffer(outRGBA);
}
