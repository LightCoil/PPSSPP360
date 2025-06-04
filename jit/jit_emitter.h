#pragma once
#include "../core/cpu_state.h"
#include <vector>
#include <cstdint>

namespace JIT {

// Низкоуровневый буфер кода (PowerPC-машинка)
struct CodeBuffer {
    std::vector<uint8_t> code;

    // Записать 32-битное слово (big-endian) — одна инструкция PowerPC
    void Emit32(uint32_t instr);
    void Emit8(uint8_t byte);

    const uint8_t* Data() const { return code.data(); }
    size_t        Size() const { return code.size(); }
};

// Класс, который «эмитирует» PowerPC-инструкции в буфер CodeBuffer
class JITEmitter {
public:
    explicit JITEmitter(CPUState &cpu);

    // Транслирует одну MIPS-инструкцию (PSP) по адресу pc, возвращает true, если удалось скомпилировать
    bool Emit(uint32_t inst, uint32_t pc);

    // Завершает блок: обычно записывает ветку возврата (blr) и отдаёт готовый буфер
    std::vector<uint8_t> Finalize();

private:
    CodeBuffer buffer_;
    CPUState  &cpu_;

    // Методы генерации конкретных PowerPC-инструкций
    void EmitMOV(uint32_t reg, uint32_t imm);   // lis/ori
    void EmitADD(uint32_t dest, uint32_t src1, uint32_t src2);
    void EmitSYSCALL();
    void EmitRET();  // blr

    // Декодер инструкции PSP → IR (упрощённый)
    bool Translate(uint32_t inst, uint32_t pc);
};

} // namespace JIT
