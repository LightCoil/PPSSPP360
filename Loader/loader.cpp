// Loader/loader.cpp

#include "loader.h"

#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <filesystem>
#include <cstdint>
#include <cstddef>

namespace ppsspp {
namespace loader {

#pragma pack(push, 1)
struct PBPHeader {
    char magic[4]; // "\0PBP"
    uint32_t version;
    uint32_t offsets[8];
};
#pragma pack(pop)

struct Elf32_Ehdr {
    uint8_t  e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
};

struct Elf32_Phdr {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
};

Loader::Loader(core::Memory& memory) : memory_(memory) {}

bool Loader::LoadPBP(const std::string& path, uint32_t& outEntry) {
    std::vector<uint8_t> fileData;
    if (!LoadFileToMemory(path, fileData)) {
        std::cerr << "Failed to read PBP file: " << path << std::endl;
        return false;
    }

    std::vector<uint8_t> elf;
    std::string titleID;

    if (!ExtractFromPBP(fileData.data(), fileData.size(), elf, titleID)) {
        std::cerr << "Invalid PBP format." << std::endl;
        return false;
    }

    std::filesystem::create_directories("saves");
    std::ofstream meta("saves/" + titleID + ".meta.txt");
    if (meta.is_open()) {
        meta << "TitleID=" << titleID << "\n";
        meta.close();
    }

    return LoadELF(elf.data(), elf.size(), outEntry);
}

bool Loader::ExtractFromPBP(const uint8_t* pbp, size_t size,
                            std::vector<uint8_t>& outELF,
                            std::string& outTitleID) {
    if (size < sizeof(PBPHeader)) return false;

    const PBPHeader* hdr = reinterpret_cast<const PBPHeader*>(pbp);
    if (std::memcmp(hdr->magic, "\0PBP", 4) != 0) return false;

    uint32_t elfStart   = hdr->offsets[0];
    uint32_t paramStart = hdr->offsets[4];

    uint32_t elfEnd = (hdr->offsets[1] < size) ? hdr->offsets[1] : (uint32_t)size;
    if (elfEnd > size || elfStart >= elfEnd) return false;

    outELF.assign(pbp + elfStart, pbp + elfEnd);

    // Read TitleID from PARAM.SFO
    if (paramStart + 512 <= size) {
        const char* param = reinterpret_cast<const char*>(pbp + paramStart);
        for (size_t i = 0; i + 8 + 9 <= 512; ++i) {
            if (std::memcmp(param + i, "TITLE_ID", 8) == 0) {
                outTitleID.assign(param + i + 8, 9);
                break;
            }
        }
    }

    return true;
}

bool Loader::LoadELF(const uint8_t* data, size_t size, uint32_t& outEntry) {
    if (size < sizeof(Elf32_Ehdr)) return false;

    const Elf32_Ehdr* hdr = reinterpret_cast<const Elf32_Ehdr*>(data);
    outEntry = hdr->e_entry;

    const Elf32_Phdr* phdrs = reinterpret_cast<const Elf32_Phdr*>(data + hdr->e_phoff);
    if ((hdr->e_phoff + hdr->e_phnum * sizeof(Elf32_Phdr)) > size)
        return false;

    for (uint16_t i = 0; i < hdr->e_phnum; ++i) {
        const Elf32_Phdr& ph = phdrs[i];
        if (ph.p_type != 1) continue;  // PT_LOAD

        if ((ph.p_offset + ph.p_filesz) > size) return false;

        memory_.WriteBytes(ph.p_vaddr, data + ph.p_offset, ph.p_filesz);
        memory_.Memset(ph.p_vaddr + ph.p_filesz, 0, ph.p_memsz - ph.p_filesz);
    }

    return true;
}

bool Loader::LoadFileToMemory(const std::string& path, std::vector<uint8_t>& out) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) return false;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size <= 0) return false;
    out.resize(static_cast<size_t>(size));
    return file.read(reinterpret_cast<char*>(out.data()), size).good();
}

}  // namespace loader
}  // namespace ppsspp
