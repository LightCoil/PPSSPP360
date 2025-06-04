#pragma once
#include <cstddef>
#include <cstdint>

struct JitBlockInfo {
    uint32_t mips_start;  // Начало блока (MIPS-адрес)
    uint32_t mips_end;    // Конец блока (MIPS-адрес, inclusive)
    uint8_t *x86_addr;    // Адрес JIT-кода в памяти Xbox360
    size_t    size;       // Размер скомпилированного блока
    uint64_t  last_used;  // Счетчик LRU (для вытеснения)
};

void InitJitCache();
void ResetJitCache();
JitBlockInfo* FindJitBlock(uint32_t mips_pc);
JitBlockInfo* AddJitBlock(uint32_t start, uint32_t end, uint8_t *code, size_t size);
void InvalidateJitBlocks(uint32_t addr);
