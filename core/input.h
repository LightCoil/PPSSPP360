#pragma once

#include "cpu_state.h"
#include <windows.h>
#include <xinput.h>
#include <array>
#include "memory.h"

namespace ppsspp {
namespace core {

// Константы для кнопок PSP
enum PSPButton {
    PSP_BUTTON_SELECT = 0,
    PSP_BUTTON_START,
    PSP_BUTTON_UP,
    PSP_BUTTON_RIGHT,
    PSP_BUTTON_DOWN,
    PSP_BUTTON_LEFT,
    PSP_BUTTON_L,
    PSP_BUTTON_R,
    PSP_BUTTON_TRIANGLE,
    PSP_BUTTON_CIRCLE,
    PSP_BUTTON_CROSS,
    PSP_BUTTON_SQUARE,
    PSP_BUTTON_HOME,
    PSP_BUTTON_HOLD,
    PSP_BUTTON_NOTE,
    PSP_BUTTON_SCREEN,
    PSP_BUTTON_VOLUP,
    PSP_BUTTON_VOLDOWN,
    PSP_BUTTON_WLAN_UP,
    PSP_BUTTON_REMOTE_HOLD,
    PSP_BUTTON_DISC,
    PSP_BUTTON_MS,
    PSP_BUTTON_COUNT
};

class InputSystem {
public:
    static InputSystem& GetInstance();

    bool Initialize();
    void Shutdown();
    void Update();
    bool IsButtonPressed(PSPButton button) const;
    bool IsButtonHeld(PSPButton button) const;
    float GetAnalogX() const { return analogX_; }
    float GetAnalogY() const { return analogY_; }
    bool IsInitialized() const { return isInitialized_; }
    void SetMemory(core::Memory* memory);
    void WriteToMemory();

private:
    InputSystem();
    ~InputSystem();

    void MapXboxToPSPButtons();
    void UpdateAnalogStick();

    // Состояние кнопок
    std::array<bool, PSP_BUTTON_COUNT> buttonState_;
    std::array<bool, PSP_BUTTON_COUNT> buttonPressed_;
    float analogX_;
    float analogY_;

    // XInput состояние
    XINPUT_STATE xInputState_;
    bool isInitialized_;
    core::Memory* memory_ = nullptr;
};

// Глобальные функции для совместимости
inline bool InitInput() {
    return InputSystem::GetInstance().Initialize();
}

inline void ShutdownInput() {
    InputSystem::GetInstance().Shutdown();
}

inline void PollInput(const CPUState& cpu) {
    InputSystem::GetInstance().Update();
}

} // namespace core
} // namespace ppsspp 