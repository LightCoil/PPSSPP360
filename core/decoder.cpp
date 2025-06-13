#include "decoder.h"

namespace ppsspp {
namespace core {

static uint8_t GetBits(uint32_t value, int hi, int lo) {
    return (value >> lo) & ((1u << (hi - lo + 1)) - 1);
}

Decoded Decoder::Decode(uint32_t pc, uint32_t word) {
    Decoded d;
    d.pc = pc;
    d.instrWord = word;

    d.opcode = GetBits(word, 31, 26);
    d.rs     = GetBits(word, 25, 21);
    d.rt     = GetBits(word, 20, 16);
    d.rd     = GetBits(word, 15, 11);
    d.shamt  = GetBits(word, 10, 6);
    d.funct  = GetBits(word, 5, 0);
    d.immediate = word & 0xFFFF;
    d.target    = ((pc + 4) & 0xF0000000) | ((word & 0x03FFFFFF) << 2);
    d.delayPC   = pc + 4;

    // VFPU fmt
    d.fmt = GetBits(word, 5, 0);

    // Delay slot определение
    switch (d.opcode) {
        case 0x01: case 0x04: case 0x05:
        case 0x06: case 0x07: case 0x02:
        case 0x03: case 0x14: case 0x15:
            d.isBranch = true;
            d.isDelaySlot = false;
            break;
        default:
            d.isBranch = false;
            d.isDelaySlot = false;
            break;
    }

    if (d.opcode == 0x10) d.cop = COP0;
    else if (d.opcode == 0x12) d.cop = COP2;
    else d.cop = NONE;

    return d;
}

} // namespace core
} // namespace ppsspp
