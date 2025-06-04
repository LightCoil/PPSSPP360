#pragma once
#include <cstdint>
#include <array>

// Состояние CPU (PSP MIPS/R4000)
struct CPUState {
    std::array<uint32_t, 32> GPR{}; // Общие регистры R0–R31
    uint32_t PC = 0;                // Программный счётчик (PC)

    // CP0 (сопроцессор 0) — статус, cause, EPC
    uint32_t CP0_STATUS = 0;
    uint32_t CP0_CAUSE  = 0;
    uint32_t CP0_EPC    = 0;

    // HI/LO для умножения/деления
    uint32_t HI = 0;
    uint32_t LO = 0;

    // FPU: 32 однословных регистров (float)
    float FPR[32] = {0.0f};

    // Флаг, что исполняется
    bool running = true;

    CPUState() {
        GPR.fill(0);
        PC = 0;
        CP0_STATUS = CP0_CAUSE = CP0_EPC = 0;
        HI = LO = 0;
        for (int i = 0; i < 32; ++i) FPR[i] = 0.0f;
        running = true;
    }
};
