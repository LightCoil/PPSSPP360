#include "hle_eboot_loader.h"
#include "../core/cpu_state.h"
#include "../core/memory.h"
#include "../core/psp_cpu.h"
#include <cstdio>

// Упрощённо: для демо просто возвращаем false
bool HLE_LoadEboot(const std::string &isoPath) {
    // Реальная логика:
    // 1. Монтировать ISO/CSO
    // 2. Найти PSP_GAME/EBOOT.PBP
    // 3. Распаковать PBP (SCE_HEADER), извлечь DATA.PSP (ELF)
    // 4. Загрузить ELF в память и установить cpu.PC
    // …здесь оставляем как stub…
    return false;
}
