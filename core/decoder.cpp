#include "decoder.h"

namespace core {

// Простейший псевдокод-декодер (не полный)
Decoded Decoder::Decode(uint32_t instr) {
    Decoded d {};
    uint32_t opcode = (instr >> 26) & 0x3F;
    switch (opcode) {
        case 0x00: {
            uint32_t funct = instr & 0x3F;
            if (funct == 0x20) { // ADD
                d.op = Op::ADD;
                d.rs = (instr >> 21) & 0x1F;
                d.rt = (instr >> 16) & 0x1F;
                d.rd = (instr >> 11) & 0x1F;
            } else if (funct == 0x0C) { // SYSCALL
                d.op = Op::SYSCALL;
            } else if (funct == 0x0D) { // BREAK
                d.op = Op::BREAK;
            } else if (funct == 0x08) { // JR
                d.op = Op::RET;
            } else {
                d.op = Op::UNKNOWN;
            }
            break;
        }
        // I‑тип
        case 0x08: // ADDI
            d.op = Op::MOV; // считать как MOV, IMM→rt
            d.rs = (instr >> 21) & 0x1F;
            d.rt = (instr >> 16) & 0x1F;
            d.imm = instr & 0xFFFF;
            break;
        default:
            d.op = Op::UNKNOWN;
            break;
    }
    return d;
}

} // namespace core
