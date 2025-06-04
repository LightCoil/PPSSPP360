#include "jit_block.h"
#include <cstdlib>
#include <cstring>

namespace JIT {

JITBlock::JITBlock(uint32_t startPC, void *compiledCode, size_t codeSize)
    : startPC_(startPC), code_(nullptr), codeSize_(codeSize) {
    code_ = std::malloc(codeSize_);
    if (code_ && compiledCode) {
        std::memcpy(code_, compiledCode, codeSize_);
    }
}

JITBlock::~JITBlock() {
    if (code_) {
        std::free(code_);
        code_ = nullptr;
    }
}

bool JITBlock::Execute(CPUState *ctx) {
    if (!code_) return false;
    using BlockFunc = void(*)(CPUState *);
    auto func = reinterpret_cast<BlockFunc>(code_);
    func(ctx);
    return true;
}

} // namespace JIT
