#pragma once
#include <cstdint>

namespace core {

enum class Op {
    MOV,
    ADD,
    SYSCALL,
    BREAK,
    RET,
    UNKNOWN
};

struct Decoded {
    Op       op;
    uint32_t rs, rt, rd;
    uint32_t imm;
};

class Decoder {
public:
    // Декодирует одну инструкцию MIPS (PSP), возвращает Decoded
    static Decoded Decode(uint32_t instr);
};

} // namespace core
