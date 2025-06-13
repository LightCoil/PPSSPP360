#pragma once

#include <xinput.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <future>
#include <chrono>
#include <functional>
#include <algorithm>

namespace xbox360 {

// Базовые типы
using XInputState = XINPUT_STATE;
using XInputVibration = XINPUT_VIBRATION;
using XInputCapabilities = XINPUT_CAPABILITIES;
using XInputBatteryInformation = XINPUT_BATTERY_INFORMATION;
using XInputKeystroke = XINPUT_KEYSTROKE;

// Структуры для статистики
struct InputStats {
    size_t controller_count;
    size_t active_controllers;
    size_t button_presses;
    size_t analog_moves;
    size_t vibration_usage;
    std::chrono::system_clock::time_point last_reset;
};

// Константы
constexpr DWORD MAX_CONTROLLERS = 4;
constexpr DWORD MAX_BUTTONS = 16;
constexpr DWORD MAX_ANALOG = 2;
constexpr DWORD MAX_VIBRATION = 2;
constexpr DWORD MAX_BATTERY = 2;

// Флаги
constexpr DWORD CONTROLLER_FLAGS = XINPUT_FLAG_GAMEPAD;
constexpr DWORD VIBRATION_FLAGS = 0;
constexpr DWORD BATTERY_FLAGS = 0;

// Пороговые значения
constexpr SHORT LEFT_THUMB_DEADZONE = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
constexpr SHORT RIGHT_THUMB_DEADZONE = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
constexpr BYTE TRIGGER_THRESHOLD = XINPUT_GAMEPAD_TRIGGER_THRESHOLD;

} // namespace xbox360 