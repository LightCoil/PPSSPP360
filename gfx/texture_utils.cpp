#include "texture_utils.h"
#include <png.h>
#include <cstdlib>
#include <cstring>
#include <xenos/xe.h>
#include <xenos/xenos.h>
#include <vector>

// Простейшая реализация: декодируем PNG через libpng, загружаем на Xenos

uint8_t* DecodePNG(const uint8_t *data, size_t size, int *outW, int *outH) {
    png_image image;
    memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;
    if (!png_image_begin_read_from_memory(&image, data, size))
        return nullptr;
    image.format = PNG_FORMAT_RGBA;
    png_bytep buffer = (png_bytep)malloc(PNG_IMAGE_SIZE(image));
    if (!buffer) return nullptr;
    if (!png_image_finish_read(&image, nullptr, buffer, 0, nullptr)) {
        free(buffer);
        return nullptr;
    }
    *outW = image.width;
    *outH = image.height;
    return buffer;
}

uint32_t UploadTexture(const uint8_t *rgba, int w, int h) {
    XenosSurface *surf = XenosSurface_Create(w, h, 4, XENON_SURFACE_FMT_8888);
    if (!surf) return 0;
    xenos_surface_upload(surf, rgba, w * 4);
    return (uint32_t)surf;
}

uint32_t LoadPNG(const std::vector<uint8_t> &pngData) {
    int w, h;
    uint8_t *rgba = DecodePNG(pngData.data(), pngData.size(), &w, &h);
    if (!rgba) return 0;
    uint32_t tex = UploadTexture(rgba, w, h);
    free(rgba);
    return tex;
}

void Texture_Draw(uint32_t texId, float x, float y, float w, float h) {
    XenosSurface *surf = (XenosSurface*)texId;
    if (!surf) return;
    // Простейший квад: (используя XenosDrawQuad)
    xenos_draw_texture(x, y, w, h, surf, 0);
}

void Texture_Free(uint32_t texId) {
    XenosSurface *surf = (XenosSurface*)texId;
    if (surf) xenos_surface_destroy(surf);
}
