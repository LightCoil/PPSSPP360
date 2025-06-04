#include "ppc_memory.h"
#include <cstring>

uint8_t ppc_memory[32 * 1024 * 1024];  // 32 MB, инициализировано в 0

uint32_t ReadMemory32(uint32_t addr) {
    if ((addr + 4) > sizeof(ppc_memory)) return 0;
    uint32_t val;
    std::memcpy(&val, &ppc_memory[addr], sizeof(uint32_t));
    return __builtin_bswap32(val);  // big-endian PPC
}

void WriteMemory32(uint32_t addr, uint32_t value) {
    if ((addr + 4) > sizeof(ppc_memory)) return;
    uint32_t be = __builtin_bswap32(value);
    std::memcpy(&ppc_memory[addr], &be, sizeof(uint32_t));
}
