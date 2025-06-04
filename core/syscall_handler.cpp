#include "syscall_handler.h"
#include "../ui/ui_manager.h"
#include "../ui/ui_pausemenu.h"
#include "../core/psp_cpu.h"

namespace core {

// Простейшая реализация: при sysc 0x20 (пауза) переходим в меню паузы
void HandleSyscall(CPUState &cpu) {
    uint32_t v0 = cpu.GPR[2]; // $v0
    switch (v0) {
        case 0x20: // Pause syscall (условный)
            // Вызвать меню паузы
            extern UIManager g_UI;
            g_UI.Push(new UIPauseMenu());
            break;
        default:
            // Неизвестный syscall: можно логировать
            break;
    }
}

} // namespace core
