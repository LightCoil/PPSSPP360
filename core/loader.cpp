#include "loader.h"
#include "ppc_memory.h"
#include <vector>
#include <cstring>

// Простейшая тестовая программа (PowerPC), не учитывается дальше
void LoadTestProgram() {
    std::vector<uint32_t> program = {
        0x38600001, // li r3,1
        0x38800002, // li r4,2
        0x38A30003, // addi r5,r3,3
        0x48000000  // b 0x0
    };
    uint32_t base = 0;
    for (size_t i = 0; i < program.size(); ++i) {
        uint32_t raw = __builtin_bswap32(program[i]);
        std::memcpy(&ppc_memory[base + i * 4], &raw, sizeof(uint32_t));
    }
}
