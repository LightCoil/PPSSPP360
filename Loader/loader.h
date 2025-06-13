// loader/loader.h

#pragma once

#include "../core/memory.h"

#include <string>
#include <vector>
#include <cstdint>

namespace ppsspp {
namespace loader {

class Loader {
public:
    explicit Loader(core::Memory& memory);

    // Загружает PBP-файл, возвращает точку входа
    bool LoadPBP(const std::string& path, uint32_t& outEntry);

private:
    // Извлекает ELF и TitleID из PBP
    bool ExtractFromPBP(const uint8_t* pbp, size_t size,
                       std::vector<uint8_t>& outELF,
                       std::string& outTitleID);

    // Загружает ELF в память
    bool LoadELF(const uint8_t* data, size_t size, uint32_t& outEntry);

    // Загружает файл в память
    bool LoadFileToMemory(const std::string& path, std::vector<uint8_t>& out);

    core::Memory& memory_;
};

}  // namespace loader
}  // namespace ppsspp
