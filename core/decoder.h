#pragma once
#include <cstdint>

namespace ppsspp {
namespace core {

// COP блоки
enum Coprocessor {
    NONE = 0,
    COP0 = 1,
    COP1 = 2,
    COP2 = 3,
    COP3 = 4,
};

// Распознанная инструкция PSP (MIPS32)
struct Decoded {
    uint32_t pc;
    uint32_t instrWord;
    uint8_t opcode;
    uint8_t rs, rt, rd;
    uint8_t shamt;
    uint8_t funct;
    uint16_t immediate;
    uint32_t target;
    Coprocessor cop;

    bool isBranch;
    bool isDelaySlot;
    uint32_t delayPC;

    uint8_t fmt; // для VFPU

    Decoded() {
        pc = 0; instrWord = 0;
        opcode = rs = rt = rd = shamt = funct = 0;
        immediate = 0; target = 0;
        isBranch = isDelaySlot = false;
        delayPC = 0;
        cop = NONE;
        fmt = 0;
    }
};

class Decoder {
public:
    Decoder(bool enableVFPU) : enableVFPU_(enableVFPU) {}

    Decoded Decode(uint32_t pc, uint32_t word);

private:
    bool enableVFPU_;
};

} // namespace core
} // namespace ppsspp
