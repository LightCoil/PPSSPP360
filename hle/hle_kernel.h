#pragma once
#include "cpu_state.h"

// Реализация HLE‐ядра: функции псевдо‐ядра PSP (например, sceKernel* )
namespace HLE {
    void InitKernel(CPUState &cpu);
    void ShutdownKernel();
    uint32_t SyscallHandler(CPUState &cpu);
};
