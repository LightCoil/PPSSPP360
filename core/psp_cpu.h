#pragma once
#include "cpu_state.h"

// Интерфейс для инициализации/запуска CPU
void CPU_Init();
void CPU_Shutdown();
void CPU_Execute(int cycles);

// Ввод с JIT или интерпретатор
void PSP_RunWithJIT(int cycles);

// Глобальное состояние CPU и память
extern CPUState cpu;
