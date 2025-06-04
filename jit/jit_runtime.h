#pragma once
#include "../core/cpu_state.h"
#include <cstdint>

namespace JIT {

// Инициализация рантайма JIT (выделение памяти и т. д.)
void InitJitRuntime();

// Сброс рантайма (очистка буфера)
void ResetJitRuntime();

// Выделяет исполняемую память (в пределах огромного JIT-буфера)
uint8_t *AllocateExecutableMemory(size_t size);

// Флаг, что при выполнении JIT нужно вернуться в интерпретатор
extern bool jitShouldExit;

// Callback из JIT-блока по прерыванию/системному вызову
void SignalExitToInterpreter();

} // namespace JIT
