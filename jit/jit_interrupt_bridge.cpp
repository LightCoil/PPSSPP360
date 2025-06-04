#include "jit_interrupt_bridge.h"
#include "jit_runtime.h"

// Прямая обвязка: при возникновении прерывания в JIT вызывается эта функция
extern "C" void JitSignalException(CPUState *cpu) {
    // Установим флаг выхода в интерпретатор
    jitShouldExit = true;
    // Сохраним необходимое состояние (если нужно)
    cpu->CP0_CAUSE |= 0x2;  // пример: пометить, что было прерывание
}
