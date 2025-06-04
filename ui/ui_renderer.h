#pragma once
#include <string>
#include <cstdint>

void UIRender_Init();
void UIRender_Begin();
void UIRender_End();
void UIRender_DrawRect(float x, float y, float w, float h, uint32_t color);
void UIRender_DrawText(const std::string &text, float x, float y, uint32_t color);
void DrawTexture(uint32_t texId, float x, float y, float w, float h);
