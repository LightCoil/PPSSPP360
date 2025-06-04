#include "save_state.h"
#include "cpu_state.h"
#include "memory.h"
#include <cstdio>
#include <ctime>
#include "../gfx/texture_utils.h"
#include "../psp_display.h"

extern CPUState cpu;
extern Memory *g_memory;

// Сохранение состояния (бинарный дамп RAM + CPUState)
void SaveStateToSlot(int slot) {
    char fname[64];
    sprintf(fname, "uda:/state%d.bin", slot);
    FILE *f = fopen(fname, "wb");
    if (!f) return;

    // Запись CPUState
    fwrite(&cpu, sizeof(CPUState), 1, f);
    // Запись всей памяти
    fwrite(PSPRam, 1, PSP_RAM_SIZE, f);
    fclose(f);

    // Сохранить скриншот
    char imgname[64];
    sprintf(imgname, "uda:/screen%d.png", slot);
    SaveScreenshot(imgname);
}

// Восстановление состояния
void LoadStateFromSlot(int slot) {
    char fname[64];
    sprintf(fname, "uda:/state%d.bin", slot);
    FILE *f = fopen(fname, "rb");
    if (!f) return;

    fread(&cpu, sizeof(CPUState), 1, f);
    fread(PSPRam, 1, PSP_RAM_SIZE, f);
    fclose(f);
}

// Сохранить текущий экран в PNG (1280×720 или 640×480)
void SaveScreenshot(const char *path) {
    int w = Display_GetWidth();
    int h = Display_GetHeight();
    int bytes = w * h * 4;
    uint8_t *buf = (uint8_t*)malloc(bytes);
    if (!buf) return;

    Display_ReadPixels(buf);      // Читает RGBA
    EncodePNGToFile(buf, w, h, path);
    free(buf);
}
