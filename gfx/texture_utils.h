#pragma once
#include <cstdint>
#include <vector>

// Загрузка PNG в память, возвращает указатель на RGBA (4 байта на пиксель)
uint8_t* DecodePNG(const uint8_t *data, size_t size, int *outW, int *outH);

// Загрузить PNG-данные в текстуру Xenos, вернуть texId
uint32_t UploadTexture(const uint8_t *rgba, int w, int h);

// Удобный хелпер: принимает вектор байт PNG и возвращает texId
uint32_t LoadPNG(const std::vector<uint8_t> &pngData);

// Нарисовать текстуру (полупрозрачный квадрат) в UI
void Texture_Draw(uint32_t texId, float x, float y, float w, float h);

// Освободить текстуру
void Texture_Free(uint32_t texId);
