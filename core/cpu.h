#ifndef CPU_H
#define CPU_H

#include "ppc_memory.h"
#include "cpu_state.h"
#include "../jit/jit_runtime.h"  // Подключаем ваш JIT-рантайм

/**
 * Класс CPU эмулирует центральный процессор PSP.
 * Внутри Step() делегирует исполнение одной инструкции JIT-движку.
 */
class CPU {
public:
    /**
     * @param mem   — объект памяти PSP (32 MB RAM)
     * @param state — структура состояния процессора (регистры, PC, флаги и т.п.)
     */
    CPU(PPCMemory& mem, CPUState& state);

    ~CPU();

    /**
     * Сбрасывает состояние CPU и JIT в исходное положение.
     */
    void Reset();

    /**
     * Выполняет одну инструкцию: JIT-движок
     * «разворачивает» код из памяти и исполняет его.
     */
    void Step();

private:
    PPCMemory& memory_;   // ссылка на эмулируемую память PSP
    CPUState&  state_;    // ссылка на объект, хранящий состояние регистров
    JitRuntime jit_;      // экземпляр JIT-рантайма (передаём туда &state_ и &memory_)
};

#endif // CPU_H
