#include "input.h"
#include <stdexcept>
#include "memory.h"

namespace ppsspp {
namespace core {

InputSystem& InputSystem::GetInstance() {
    static InputSystem instance;
    return instance;
}

InputSystem::InputSystem()
    : analogX_(0.0f)
    , analogY_(0.0f)
    , isInitialized_(false) {
    
    buttonState_.fill(false);
    buttonPressed_.fill(false);
}

InputSystem::~InputSystem() {
    Shutdown();
}

bool InputSystem::Initialize() {
    if (isInitialized_) {
        return true;
    }

    // Инициализация XInput
    DWORD result = XInputGetState(0, &xInputState_);
    if (result != ERROR_SUCCESS) {
        return false;
    }

    isInitialized_ = true;
    return true;
}

void InputSystem::Shutdown() {
    if (!isInitialized_) {
        return;
    }

    buttonState_.fill(false);
    buttonPressed_.fill(false);
    analogX_ = 0.0f;
    analogY_ = 0.0f;
    isInitialized_ = false;
}

void InputSystem::Update() {
    if (!isInitialized_) {
        return;
    }

    // Получение состояния геймпада
    DWORD result = XInputGetState(0, &xInputState_);
    if (result != ERROR_SUCCESS) {
        return;
    }

    // Обновление состояния кнопок
    MapXboxToPSPButtons();

    // Обновление аналогового стика
    UpdateAnalogStick();

    WriteToMemory();
}

void InputSystem::MapXboxToPSPButtons() {
    // Сохраняем предыдущее состояние
    buttonPressed_ = buttonState_;

    // Маппинг кнопок Xbox на PSP
    buttonState_[PSP_BUTTON_CROSS] = (xInputState_.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
    buttonState_[PSP_BUTTON_CIRCLE] = (xInputState_.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;
    buttonState_[PSP_BUTTON_SQUARE] = (xInputState_.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0;
    buttonState_[PSP_BUTTON_TRIANGLE] = (xInputState_.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;
    buttonState_[PSP_BUTTON_L] = (xInputState_.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
    buttonState_[PSP_BUTTON_R] = (xInputState_.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
    buttonState_[PSP_BUTTON_SELECT] = (xInputState_.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0;
    buttonState_[PSP_BUTTON_START] = (xInputState_.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0;

    // D-pad
    buttonState_[PSP_BUTTON_UP] = (xInputState_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
    buttonState_[PSP_BUTTON_DOWN] = (xInputState_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
    buttonState_[PSP_BUTTON_LEFT] = (xInputState_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
    buttonState_[PSP_BUTTON_RIGHT] = (xInputState_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
}

void InputSystem::UpdateAnalogStick() {
    // Нормализация значений аналогового стика
    float x = xInputState_.Gamepad.sThumbLX / 32768.0f;
    float y = xInputState_.Gamepad.sThumbLY / 32768.0f;

    // Применение мертвой зоны
    const float deadzone = 0.1f;
    if (std::abs(x) < deadzone) x = 0.0f;
    if (std::abs(y) < deadzone) y = 0.0f;

    analogX_ = x;
    analogY_ = y;
}

bool InputSystem::IsButtonPressed(PSPButton button) const {
    if (button >= PSP_BUTTON_COUNT) {
        return false;
    }
    return buttonState_[button] && !buttonPressed_[button];
}

bool InputSystem::IsButtonHeld(PSPButton button) const {
    if (button >= PSP_BUTTON_COUNT) {
        return false;
    }
    return buttonState_[button];
}

void InputSystem::SetMemory(core::Memory* memory) {
    memory_ = memory;
}

void InputSystem::WriteToMemory() {
    if (!memory_) return;
    // Структура SceCtrlData PSP (упрощённо)
    struct SceCtrlData {
        uint32_t buttons;
        uint8_t lx;
        uint8_t ly;
        uint8_t reserved[6];
    } data{};
    // Маппинг PSP кнопок в битовую маску
    for (int i = 0; i < PSP_BUTTON_COUNT; ++i) {
        if (buttonState_[i])
            data.buttons |= (1 << i);
    }
    // Аналоговые стики (0-255)
    data.lx = static_cast<uint8_t>((analogX_ * 127.0f) + 128.0f);
    data.ly = static_cast<uint8_t>((analogY_ * 127.0f) + 128.0f);
    memory_->Write32(0x88000000, data.buttons);
    memory_->Write8(0x88000004, data.lx);
    memory_->Write8(0x88000005, data.ly);
    // Остальные байты обнуляем
    for (int i = 0; i < 6; ++i)
        memory_->Write8(0x88000006 + i, 0);
}

} // namespace core
} // namespace ppsspp 