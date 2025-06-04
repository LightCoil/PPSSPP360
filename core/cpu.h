#ifndef CPU_H
#define CPU_H

#include "ppc_memory.h"   // чтобы был доступ к PPCMemory
#include "cpu_state.h"    // чтобы был доступ к CPUState

class CPU {
public:
    // Раньше могло стоять: CPU(Memory &mem, CPUState &state);
    // Здесь заменили Memory на PPCMemory:
    CPU(PPCMemory& mem, CPUState& state);
    ~CPU();

    // Основные методы эмуляции
    void Step();       // выполнить один шаг (инструкцию)
    void Reset();

private:
    PPCMemory& memory_;
    CPUState&  state_;
};

#endif // CPU_H
