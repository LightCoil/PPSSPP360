#include "cpu.h"
#include <iostream>
#include <cassert>

namespace core {

CPU::CPU(Memory &mem, CPUState &state)
    : memory_(mem), state_(state) {
    state_.PC = 0x00000000;
}

uint32_t CPU::FetchInstruction() {
    return memory_.Read32(state_.PC);
}

void CPU::DecodeAndExecute(uint32_t instr) {
    uint32_t opcode = (instr >> 26) & 0x3F;
    if (opcode == 0x00) {
        ExecuteSPECIAL(instr);
    } else if (opcode == 0x01) {
        ExecuteREGIMM(instr);
    } else if (opcode == 0x02 || opcode == 0x03) {
        ExecuteJType(instr);
    } else {
        ExecuteIType(instr);
    }
}

void CPU::ExecuteInstruction() {
    uint32_t instr = FetchInstruction();
    DecodeAndExecute(instr);
    state_.PC += 4;
}

void CPU::Run(int cycles) {
    for (int i = 0; i < cycles && state_.running; ++i) {
        ExecuteInstruction();
    }
}

// ----------------------
// SPECIAL (opcode == 0)
void CPU::ExecuteSPECIAL(uint32_t instr) {
    uint32_t funct = instr & 0x3F;
    int rs = (instr >> 21) & 0x1F;
    int rt = (instr >> 16) & 0x1F;
    int rd = (instr >> 11) & 0x1F;
    int sa = (instr >> 6)  & 0x1F;

    switch (funct) {
        case 0x00: // SLL rd, rt, sa
            GPR(rd) = GPR(rt) << sa;
            break;

        case 0x20: { // ADD rd, rs, rt
            int32_t a = static_cast<int32_t>(GPR(rs));
            int32_t b = static_cast<int32_t>(GPR(rt));
            GPR(rd) = static_cast<uint32_t>(a + b);
            break;
        }

        case 0x08: { // JR rs
            state_.PC = GPR(rs) - 4;
            break;
        }

        case 0x09: { // JALR rd, rs
            GPR(rd) = state_.PC + 4;
            state_.PC = GPR(rs) - 4;
            break;
        }

        case 0x0C: // SYSCALL
            state_.CP0_CAUSE = 8;
            state_.CP0_EPC = state_.PC;
            state_.CP0_STATUS |= 0x2;
            state_.PC = 0x80000080;
            break;

        case 0x0D: // BREAK
            state_.CP0_CAUSE = 9;
            state_.CP0_EPC = state_.PC;
            state_.CP0_STATUS |= 0x2;
            state_.PC = 0x80000080;
            break;

        case 0x18: { // MULT
            int32_t a = static_cast<int32_t>(GPR(rs));
            int32_t b = static_cast<int32_t>(GPR(rt));
            int64_t res = static_cast<int64_t>(a) * b;
            state_.HI = static_cast<uint32_t>((res >> 32) & 0xFFFFFFFF);
            state_.LO = static_cast<uint32_t>(res & 0xFFFFFFFF);
            break;
        }

        case 0x19: { // MULTU
            uint64_t a = GPR(rs);
            uint64_t b = GPR(rt);
            uint64_t res = a * b;
            state_.HI = static_cast<uint32_t>((res >> 32) & 0xFFFFFFFF);
            state_.LO = static_cast<uint32_t>(res & 0xFFFFFFFF);
            break;
        }

        case 0x1A: { // DIV
            int32_t a = static_cast<int32_t>(GPR(rs));
            int32_t b = static_cast<int32_t>(GPR(rt));
            if (b == 0) {
                state_.HI = 0;
                state_.LO = (a >= 0) ? 0xFFFFFFFF : 1;
            } else {
                state_.LO = static_cast<uint32_t>(a / b);
                state_.HI = static_cast<uint32_t>(a % b);
            }
            break;
        }

        case 0x1B: { // DIVU
            uint32_t a = GPR(rs);
            uint32_t b = GPR(rt);
            if (b == 0) {
                state_.HI = 0;
                state_.LO = 0xFFFFFFFF;
            } else {
                state_.LO = a / b;
                state_.HI = a % b;
            }
            break;
        }

        case 0x10: { // MFHI
            GPR(rd) = state_.HI;
            break;
        }

        case 0x12: { // MFLO
            GPR(rd) = state_.LO;
            break;
        }

        case 0x11: { // MTHI
            state_.HI = GPR(rs);
            break;
        }

        case 0x13: { // MTLO
            state_.LO = GPR(rs);
            break;
        }

        default:
            std::cerr << "Unimplemented SPECIAL funct: 0x" << std::hex << funct << std::dec << "\n";
            break;
    }
}

// REGIMM (opcode == 1)
void CPU::ExecuteREGIMM(uint32_t instr) {
    uint32_t rt = (instr >> 16) & 0x1F;
    int rs = (instr >> 21) & 0x1F;
    int16_t offset = static_cast<int16_t>(instr & 0xFFFF);
    uint32_t target = state_.PC + 4 + (static_cast<int32_t>(offset) << 2);

    switch (rt) {
        case 0x00: { // BLTZ
            int32_t v = static_cast<int32_t>(GPR(rs));
            if (v < 0) state_.PC = target - 4;
            break;
        }
        case 0x01: { // BGEZ
            int32_t v = static_cast<int32_t>(GPR(rs));
            if (v >= 0) state_.PC = target - 4;
            break;
        }
        case 0x10: { // BLTZAL
            int32_t v = static_cast<int32_t>(GPR(rs));
            GPR(31) = state_.PC + 4;
            if (v < 0) state_.PC = target - 4;
            break;
        }
        case 0x11: { // BGEZAL
            int32_t v = static_cast<int32_t>(GPR(rs));
            GPR(31) = state_.PC + 4;
            if (v >= 0) state_.PC = target - 4;
            break;
        }
        default:
            std::cerr << "Unimplemented REGIMM rt: 0x" << std::hex << rt << std::dec << "\n";
            break;
    }
}

// J‑тип (opcode 2,3)
void CPU::ExecuteJType(uint32_t instr) {
    uint32_t opcode = (instr >> 26) & 0x3F;
    uint32_t index  = instr & 0x03FFFFFF;
    uint32_t target = ((state_.PC + 4) & 0xF0000000) | (index << 2);

    if (opcode == 0x02) { // J
        state_.PC = target - 4;
    } else if (opcode == 0x03) { // JAL
        GPR(31) = state_.PC + 4;
        state_.PC = target - 4;
    }
}

// I‑тип
void CPU::ExecuteIType(uint32_t instr) {
    uint32_t opcode = (instr >> 26) & 0x3F;
    int rs = (instr >> 21) & 0x1F;
    int rt = (instr >> 16) & 0x1F;
    int16_t imm = static_cast<int16_t>(instr & 0xFFFF);

    switch (opcode) {
        case 0x08: { // ADDI
            int32_t a = static_cast<int32_t>(GPR(rs));
            GPR(rt) = static_cast<uint32_t>(a + imm);
            break;
        }
        case 0x09: { // ADDIU
            GPR(rt) = GPR(rs) + static_cast<int32_t>(imm);
            break;
        }
        case 0x0C: { // ANDI
            GPR(rt) = GPR(rs) & static_cast<uint16_t>(imm);
            break;
        }
        case 0x0D: { // ORI
            GPR(rt) = GPR(rs) | static_cast<uint16_t>(imm);
            break;
        }
        case 0x0E: { // XORI
            GPR(rt) = GPR(rs) ^ static_cast<uint16_t>(imm);
            break;
        }
        case 0x0A: { // SLTI
            int32_t a = static_cast<int32_t>(GPR(rs));
            GPR(rt) = (a < imm) ? 1 : 0;
            break;
        }
        case 0x0B: { // SLTIU
            uint32_t a = GPR(rs);
            GPR(rt) = (a < static_cast<uint32_t>(imm)) ? 1 : 0;
            break;
        }
        case 0x0F: { // LUI
            GPR(rt) = static_cast<uint32_t>(imm) << 16;
            break;
        }
        case 0x04: { // BEQ
            if (GPR(rs) == GPR(rt)) {
                state_.PC = state_.PC + 4 + (static_cast<int32_t>(imm) << 2) - 4;
            }
            break;
        }
        case 0x05: { // BNE
            if (GPR(rs) != GPR(rt)) {
                state_.PC = state_.PC + 4 + (static_cast<int32_t>(imm) << 2) - 4;
            }
            break;
        }
        case 0x23: { // LW
            uint32_t addr = GPR(rs) + static_cast<int32_t>(imm);
            if (addr % 4 != 0) std::cerr << "LW: Unaligned 0x" << std::hex << addr << "\n";
            GPR(rt) = memory_.Read32(addr);
            break;
        }
        case 0x2B: { // SW
            uint32_t addr = GPR(rs) + static_cast<int32_t>(imm);
            if (addr % 4 != 0) std::cerr << "SW: Unaligned 0x" << std::hex << addr << "\n";
            memory_.Write32(addr, GPR(rt));
            break;
        }
        case 0x20: { // LB
            uint32_t addr = GPR(rs) + static_cast<int32_t>(imm);
            int8_t val = static_cast<int8_t>(memory_.Read8(addr));
            GPR(rt) = static_cast<uint32_t>(static_cast<int32_t>(val));
            break;
        }
        case 0x24: { // LBU
            uint32_t addr = GPR(rs) + static_cast<int32_t>(imm);
            GPR(rt) = memory_.Read8(addr);
            break;
        }
        case 0x28: { // SB
            uint32_t addr = GPR(rs) + static_cast<int32_t>(imm);
            memory_.Write8(addr, static_cast<uint8_t>(GPR(rt) & 0xFF));
            break;
        }
        case 0x21: { // LH
            uint32_t addr = GPR(rs) + static_cast<int32_t>(imm);
            if (addr % 2 != 0) std::cerr << "LH: Unaligned 0x" << std::hex << addr << "\n";
            int16_t val = static_cast<int16_t>(memory_.Read16(addr));
            GPR(rt) = static_cast<uint32_t>(static_cast<int32_t>(val));
            break;
        }
        case 0x25: { // LHU
            uint32_t addr = GPR(rs) + static_cast<int32_t>(imm);
            if (addr % 2 != 0) std::cerr << "LHU: Unaligned 0x" << std::hex << addr << "\n";
            GPR(rt) = memory_.Read16(addr);
            break;
        }
        case 0x29: { // SH
            uint32_t addr = GPR(rs) + static_cast<int32_t>(imm);
            if (addr % 2 != 0) std::cerr << "SH: Unaligned 0x" << std::hex << addr << "\n";
            memory_.Write16(addr, static_cast<uint16_t>(GPR(rt) & 0xFFFF));
            break;
        }
        default:
            std::cerr << "Unimplemented I-type opcode: 0x" << std::hex << opcode << std::dec << "\n";
            break;
    }
}

} // namespace core
