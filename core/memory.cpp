#include "memory.h"
#include "logger.h"
#include <cstring>
#include <stdexcept>

namespace ppsspp {
namespace core {

Memory::Memory() : ramSize_(0) {
    InitializeMemory();
}

bool Memory::Init() {
    try {
        InitializeMemory();
        return true;
    } catch (const std::exception& e) {
        LogError("Failed to initialize memory: " + std::string(e.what()));
        return false;
    }
}

void Memory::InitializeMemory() {
    ramSize_ = 32 * 1024 * 1024; // 32MB RAM
    ram_ = std::make_unique<uint8_t[]>(ramSize_);
    if (!ram_) {
        throw MemoryError("Failed to allocate memory");
    }
    std::memset(ram_.get(), 0, ramSize_);
    LogInfo("Memory initialized with " + std::to_string(ramSize_) + " bytes");
}

void Memory::CheckBounds(uint32_t addr, size_t size) const {
    if (!ram_) {
        throw MemoryError("Memory not initialized");
    }
    if (addr + size > ramSize_) {
        throw MemoryError("Memory access out of bounds: addr=" + 
            std::to_string(addr) + ", size=" + std::to_string(size));
    }
}

bool Memory::IsAligned(uint32_t addr, size_t alignment) const {
    return (addr % alignment) == 0;
}

uint8_t Memory::Read8(uint32_t addr) const {
    CheckBounds(addr, sizeof(uint8_t));
    return ram_[addr];
}

uint16_t Memory::Read16(uint32_t addr) const {
    CheckBounds(addr, sizeof(uint16_t));
    if (!IsAligned(addr, sizeof(uint16_t))) {
        LogWarning("Unaligned 16-bit read at address " + std::to_string(addr));
    }
    return *reinterpret_cast<const uint16_t*>(&ram_[addr]);
}

uint32_t Memory::Read32(uint32_t addr) const {
    CheckBounds(addr, sizeof(uint32_t));
    if (!IsAligned(addr, sizeof(uint32_t))) {
        LogWarning("Unaligned 32-bit read at address " + std::to_string(addr));
    }
    return *reinterpret_cast<const uint32_t*>(&ram_[addr]);
}

void Memory::Write8(uint32_t addr, uint8_t value) {
    CheckBounds(addr, sizeof(uint8_t));
    ram_[addr] = value;
}

void Memory::Write16(uint32_t addr, uint16_t value) {
    CheckBounds(addr, sizeof(uint16_t));
    if (!IsAligned(addr, sizeof(uint16_t))) {
        LogWarning("Unaligned 16-bit write at address " + std::to_string(addr));
    }
    *reinterpret_cast<uint16_t*>(&ram_[addr]) = value;
}

void Memory::Write32(uint32_t addr, uint32_t value) {
    CheckBounds(addr, sizeof(uint32_t));
    if (!IsAligned(addr, sizeof(uint32_t))) {
        LogWarning("Unaligned 32-bit write at address " + std::to_string(addr));
    }
    *reinterpret_cast<uint32_t*>(&ram_[addr]) = value;
}

void Memory::WriteBytes(uint32_t addr, const void* data, size_t size) {
    if (!data) {
        throw MemoryError("Null pointer passed to WriteBytes");
    }
    CheckBounds(addr, size);
    std::memcpy(&ram_[addr], data, size);
}

void Memory::Memset(uint32_t addr, uint8_t value, size_t size) {
    CheckBounds(addr, size);
    std::memset(&ram_[addr], value, size);
}

const uint8_t* Memory::GetPointer(uint32_t addr) const {
    CheckBounds(addr, 1);
    return &ram_[addr];
}

uint8_t* Memory::GetPointer(uint32_t addr) {
    CheckBounds(addr, 1);
    return &ram_[addr];
}

} // namespace core
} // namespace ppsspp
