#include "jit_emitter.h"
#include "../core/decoder.h"  // Декодер MIPS-инструкций (псевдокод)
#include <cstring>

namespace JIT {

void CodeBuffer::Emit32(uint32_t instr) {
    code.push_back((instr >> 24) & 0xFF);
    code.push_back((instr >> 16) & 0xFF);
    code.push_back((instr >> 8)  & 0xFF);
    code.push_back(instr & 0xFF);
}

void CodeBuffer::Emit8(uint8_t byte) {
    code.push_back(byte);
}

JITEmitter::JITEmitter(CPUState &cpu)
    : cpu_(cpu) {}

bool JITEmitter::Emit(uint32_t inst, uint32_t pc) {
    return Translate(inst, pc);
}

std::vector<uint8_t> JITEmitter::Finalize() {
    EmitRET();      // blr — возврат из JIT-блока
    return buffer_.code;
}

// ------------------------
// Реализация Translate:
bool JITEmitter::Translate(uint32_t inst, uint32_t pc) {
    auto decoded = core::Decoder::Decode(inst);

    switch (decoded.op) {
        case core::Op::MOV:  // li rD, IMM
            EmitMOV(decoded.rd, decoded.imm);
            return true;
        case core::Op::ADD:
            EmitADD(decoded.rd, decoded.rs, decoded.rt);
            return true;
        case core::Op::SYSCALL:
            EmitSYSCALL();
            return false;  // блок закончится
        // … дописать остальные необходимые PSP-инструкции …
        default:
            return false;
    }
}

// Генерация PowerPC кода:
void JITEmitter::EmitMOV(uint32_t reg, uint32_t imm) {
    // lis reg, imm[31:16]
    buffer_.Emit32(0x3C000000 | (reg << 21) | ((imm >> 16) & 0xFFFF));
    // ori reg, reg, imm[15:0]
    buffer_.Emit32(0x60000000 | (reg << 21) | (reg << 16) | (imm & 0xFFFF));
}

void JITEmitter::EmitADD(uint32_t dest, uint32_t src1, uint32_t src2) {
    buffer_.Emit32(0x7C000214 | (src1 << 16) | (src2 << 11) | (dest << 21));
}

void JITEmitter::EmitSYSCALL() {
    buffer_.Emit32(0x44000002); // sc
}

void JITEmitter::EmitRET() {
    buffer_.Emit32(0x4E800020); // blr
}

} // namespace JIT
