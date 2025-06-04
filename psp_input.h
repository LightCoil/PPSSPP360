#pragma once
#include <cstdint>

void Input_Update();
uint32_t Input_GetButtons();

// Кнопки (примерные значения)
#define BTN_A      (1u << 0)
#define BTN_B      (1u << 1)
#define BTN_UP     (1u << 2)
#define BTN_DOWN   (1u << 3)
#define BTN_LEFT   (1u << 4)
#define BTN_RIGHT  (1u << 5)
#define BTN_START  (1u << 6)
#define BTN_SELECT (1u << 7)
#define BTN_L      (1u << 8)
#define BTN_R      (1u << 9)
#define BTN_BACK   (1u << 10)
