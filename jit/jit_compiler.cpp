#include "jit_compiler.h"
#include "jit_emitter.h"
#include "../core/memory.h"
#include "../core/decoder.h"
#include <cstring>

namespace JIT {

JITCompiler::JITCompiler(JITBlockCache &cache, CPUState &cpu)
    : cache_(cache), cpu_(cpu) {}

JITBlock* JITCompiler::Compile(uint32_t pc) {
    if (auto *b = cache_.Find(pc)) {
        return b;
    }
    auto block = GenerateBlock(pc);
    JITBlock *result = block.get();
    cache_.Insert(pc, std::move(block));
    return result;
}

std::unique_ptr<JITBlock> JITCompiler::GenerateBlock(uint32_t pc) {
    constexpr int MAX_INSTRUCTIONS = 64;
    uint32_t insts[MAX_INSTRUCTIONS];
    size_t count = 0;
    uint32_t current = pc;

    while (count < MAX_INSTRUCTIONS) {
        uint32_t op = core::Memory::Read32(current);
        insts[count++] = op;
        current += 4;
        auto dec = core::Decoder::Decode(op);
        if (dec.op == core::Op::SYSCALL || dec.op == core::Op::BREAK || dec.op == core::Op::RET) {
            break;
        }
    }

    std::vector<uint8_t> codeBytes;
    {
        JITEmitter emitter(cpu_);
        for (size_t i = 0; i < count; ++i) {
            emitter.Emit(insts[i], pc + i * 4);
        }
        codeBytes = emitter.Finalize();
    }

    void *mem = JIT::AllocateExecutableMemory(codeBytes.size());
    std::memcpy(mem, codeBytes.data(), codeBytes.size());
    return std::make_unique<JITBlock>(pc, mem, codeBytes.size());
}

} // namespace JIT
