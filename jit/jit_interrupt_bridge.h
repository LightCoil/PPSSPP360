#pragma once
#include "../core/cpu_state.h"

namespace JIT {

// Эта функция вызывается из JIT-блока при SYSCALL/BREAK/прерывании
extern "C" void JitSignalException(CPUState *cpu);

} // namespace JIT
