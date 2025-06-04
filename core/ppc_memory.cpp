#include "ppc_memory.h"
#include <cassert>
#include <cstring>

PPCMemory::PPCMemory() {
}

PPCMemory::~PPCMemory() {
    Free();
}

bool PPCMemory::Allocate(size_t size) {
    try {
        memory_.resize(size);
    } catch (...) {
        return false;
    }
    return true;
}

void PPCMemory::Free() {
    if (!memory_.empty()) {
        memory_.clear();
        memory_.shrink_to_fit();
    }
}

uint32_t PPCMemory::ReadU32(uint32_t addr) const {
    assert(addr + 3 < memory_.size());
    // PSP — Little Endian
    uint32_t v;
    std::memcpy(&v, &memory_[addr], sizeof(uint32_t));
    return v;
}

void PPCMemory::WriteU32(uint32_t addr, uint32_t value) {
    assert(addr + 3 < memory_.size());
    std::memcpy(&memory_[addr], &value, sizeof(uint32_t));
}

uint16_t PPCMemory::ReadU16(uint32_t addr) const {
    assert(addr + 1 < memory_.size());
    uint16_t v;
    std::memcpy(&v, &memory_[addr], sizeof(uint16_t));
    return v;
}

void PPCMemory::WriteU16(uint32_t addr, uint16_t value) {
    assert(addr + 1 < memory_.size());
    std::memcpy(&memory_[addr], &value, sizeof(uint16_t));
}

uint8_t PPCMemory::ReadU8(uint32_t addr) const {
    assert(addr < memory_.size());
    return memory_[addr];
}

void PPCMemory::WriteU8(uint32_t addr, uint8_t value) {
    assert(addr < memory_.size());
    memory_[addr] = value;
}
