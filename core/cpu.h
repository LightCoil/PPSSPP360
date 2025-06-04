#pragma once
#include <cstdint>
#include "memory.h"
#include "cpu_state.h"

namespace core {

class CPU {
public:
    CPU(Memory &mem, CPUState &state);
    void ExecuteInstruction();       // Интерпретатор: выполнить 1 инструкцию
    void Run(int cycles);            // Запустить интерпретатор на N инструкций

private:
    Memory &memory_;
    CPUState &state_;

    uint32_t FetchInstruction();
    void DecodeAndExecute(uint32_t instr);

    // Группы инструкций
    void ExecuteSPECIAL(uint32_t instr);
    void ExecuteREGIMM(uint32_t instr);
    void ExecuteJType(uint32_t instr);
    void ExecuteIType(uint32_t instr);

    // Вспомогательные для работы с регистрами
    uint32_t &GPR(int index) { return state_.GPR[index]; }
};

} // namespace core
