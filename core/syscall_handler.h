#pragma once
#include "cpu_state.h"

namespace core {

// Обработка системных вызовов (PSP ABI)
void HandleSyscall(CPUState &cpu);

} // namespace core
