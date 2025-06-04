#pragma once
#include <cstdint>

extern uint8_t ppc_memory[32 * 1024 * 1024];  // 32 MB

uint32_t ReadMemory32(uint32_t addr);
void     WriteMemory32(uint32_t addr, uint32_t value);
