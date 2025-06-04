#include "hle_kernel.h"
#include <iostream>

namespace HLE {

static CPUState *g_cpu = nullptr;

void InitKernel(CPUState &cpu) {
    g_cpu = &cpu;
    // Инициализировать структуры HLE‐ядра (stub)
}

void ShutdownKernel() {
    g_cpu = nullptr;
}

// Обработка HLE‐системных вызовов
uint32_t SyscallHandler(CPUState &cpu) {
    // Проверим код из регистра $v0 (cpu.GPR[2]) и эмулируем поведение PSP
    uint32_t v0 = cpu.GPR[2];
    switch (v0) {
        // Например, 0x0B = sceKernelExitGame
        case 0x0B:
            cpu.running = false;
            break;
        default:
            // stub: ничего не делаем
            break;
    }
    return 0;
}

} // namespace HLE
