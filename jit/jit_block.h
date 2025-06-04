#pragma once
#include <cstddef>
#include <cstdint>
#include "../core/cpu_state.h"

namespace JIT {

class JITBlock {
public:
    JITBlock(uint32_t startPC, void *compiledCode, size_t codeSize);
    ~JITBlock();

    bool Execute(CPUState *ctx);

    uint32_t GetStartPC() const { return startPC_; }
    size_t   GetSize()    const { return codeSize_; }
    void*    GetCode()    const { return code_; }

    int hits = 0;        // Счётчик вхождений
    bool inlined = false;

private:
    uint32_t startPC_;
    void    *code_;
    size_t   codeSize_;
};

} // namespace JIT
