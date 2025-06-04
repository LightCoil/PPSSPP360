#pragma once
#include <string>

// Загрузить EBOOT.PBP из ISO, распаковать, загрузить ELF внутрь PSP памяти
bool HLE_LoadEboot(const std::string &isoPath);
