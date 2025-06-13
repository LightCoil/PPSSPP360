#include "syscall.h"
#include "syscall_defs.h"
#include "cpu_state.h"
#include "memory.h"
#include <stdexcept>
#include <ctime>

namespace ppsspp {
namespace core {

void HandleSyscall(CPUState* st, uint32_t syscallID) {
    switch (syscallID) {
    case SYSCALL_EXIT:
        // Выставляем флаг завершения
        st->SetPC(CPUState::INVALID_PC);
        break;

    case SYSCALL_GET_TIME: {
        std::time_t now = std::time(nullptr);
        st->SetGPR(2, uint32_t(now)); // возвращаем в v0
        break;
    }

    case SYSCALL_CTRL_INPUT:
        // Пример ввода кнопок
        st->SetGPR(2, 0x00008000); // напр., кнопка Start
        break;

    default:
        // Неизвестный syscall
        break;
    }
}

uint32_t HandleSyscall(CPUState& cpu, uint32_t syscall_num) {
    // Получаем значения регистров
    uint32_t a0 = cpu.GetGPR(4);  // $a0
    uint32_t a1 = cpu.GetGPR(5);  // $a1
    uint32_t a2 = cpu.GetGPR(6);  // $a2
    uint32_t a3 = cpu.GetGPR(7);  // $a3

    switch (syscall_num) {
        case SYSCALL_EXIT: // sceKernelExitGame
            // TODO: Реализовать выход из игры
            return 0;

        case SYSCALL_EXIT_THREAD: // sceKernelExitThread
            // TODO: Реализовать выход из потока
            return 0;

        case SYSCALL_LOAD_EXEC: // sceKernelLoadExec
            // TODO: Реализовать загрузку и выполнение
            return 0;

        case SYSCALL_REGISTER_LIB: // sceKernelRegisterLibraryForUser
            // TODO: Реализовать регистрацию библиотеки
            return 0;

        case SYSCALL_CREATE_THREAD: // sceKernelCreateThread
            // TODO: Реализовать создание потока
            return 0;

        case SYSCALL_START_THREAD: // sceKernelStartThread
            // TODO: Реализовать запуск потока
            return 0;

        case SYSCALL_EXIT_DELETE_THREAD: // sceKernelExitDeleteThread
            // TODO: Реализовать выход и удаление потока
            return 0;

        case SYSCALL_SLEEP_THREAD: // sceKernelSleepThread
            // TODO: Реализовать приостановку потока
            return 0;

        case SYSCALL_WAKEUP_THREAD: // sceKernelWakeupThread
            // TODO: Реализовать пробуждение потока
            return 0;

        case SYSCALL_DELAY_THREAD: // sceKernelDelayThread
            // TODO: Реализовать задержку потока
            return 0;

        case SYSCALL_GET_TIME: // Получение времени
            {
                auto now = std::time(nullptr);
                return static_cast<uint32_t>(now);
            }

        case SYSCALL_CTRL_INPUT: // Ввод с контроллера
            // TODO: Реализовать ввод с контроллера
            return 0;

        default:
            throw std::runtime_error("Unknown syscall number: " + std::to_string(syscall_num));
    }
}

}  // namespace core
}  // namespace ppsspp
