#pragma once
#include "jit_block_cache.h"
#include "../core/cpu_state.h"

namespace JIT {

class JITCompiler {
public:
    JITCompiler(JITBlockCache &cache, CPUState &cpu);

    // Компилирует блок, начинающийся с pc, возвращает готовый JITBlock*
    JITBlock* Compile(uint32_t pc);

private:
    JITBlockCache &cache_;
    CPUState       &cpu_;

    // Генерирует новый блок, если в кэше нет
    std::unique_ptr<JITBlock> GenerateBlock(uint32_t pc);
};

} // namespace JIT
