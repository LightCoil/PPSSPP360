#include "jit_cache_xbox360.h"
#include <cstdint>
#include <cstring>
#include <climits>

static constexpr size_t MAX_JIT_BLOCKS = 8192;
static JitBlockInfo jit_blocks[MAX_JIT_BLOCKS];
static uint64_t global_counter = 0;

void InitJitCache() {
    ResetJitCache();
}

void ResetJitCache() {
    global_counter = 0;
    for (size_t i = 0; i < MAX_JIT_BLOCKS; ++i) {
        jit_blocks[i].x86_addr    = nullptr;
        jit_blocks[i].mips_start  = 0;
        jit_blocks[i].mips_end    = 0;
        jit_blocks[i].size        = 0;
        jit_blocks[i].last_used   = 0;
    }
}

JitBlockInfo* FindJitBlock(uint32_t mips_pc) {
    for (size_t i = 0; i < MAX_JIT_BLOCKS; ++i) {
        auto &b = jit_blocks[i];
        if (b.x86_addr != nullptr && mips_pc >= b.mips_start && mips_pc <= b.mips_end) {
            b.last_used = ++global_counter;
            return &b;
        }
    }
    return nullptr;
}

JitBlockInfo* AddJitBlock(uint32_t start, uint32_t end, uint8_t *code, size_t size) {
    size_t lru_index = 0;
    uint64_t lru_min = UINT64_MAX;
    for (size_t i = 0; i < MAX_JIT_BLOCKS; ++i) {
        if (jit_blocks[i].x86_addr == nullptr) {
            lru_index = i;
            break;
        }
        if (jit_blocks[i].last_used < lru_min) {
            lru_min = jit_blocks[i].last_used;
            lru_index = i;
        }
    }
    jit_blocks[lru_index].mips_start = start;
    jit_blocks[lru_index].mips_end   = end;
    jit_blocks[lru_index].x86_addr   = code;
    jit_blocks[lru_index].size       = size;
    jit_blocks[lru_index].last_used  = ++global_counter;
    return &jit_blocks[lru_index];
}

void InvalidateJitBlocks(uint32_t addr) {
    for (size_t i = 0; i < MAX_JIT_BLOCKS; ++i) {
        auto &b = jit_blocks[i];
        if (b.x86_addr && addr >= b.mips_start && addr <= b.mips_end) {
            b.x86_addr = nullptr;
        }
    }
}
