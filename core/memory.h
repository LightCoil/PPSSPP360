#pragma once
#include <cstdint>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>

namespace ppsspp {
namespace core {

class MemoryError : public std::runtime_error {
public:
    explicit MemoryError(const std::string& message) : std::runtime_error(message) {}
    virtual ~MemoryError() = default;
};

class Memory {
public:
    Memory();
    ~Memory() = default;

    // Запрещаем копирование
    Memory(const Memory&) = delete;
    Memory& operator=(const Memory&) = delete;

    // Разрешаем перемещение
    Memory(Memory&&) noexcept = default;
    Memory& operator=(Memory&&) noexcept = default;

    bool Init();

    // Методы чтения
    uint8_t  Read8(uint32_t addr) const;
    uint16_t Read16(uint32_t addr) const;
    uint32_t Read32(uint32_t addr) const;

    // Методы записи
    void Write8(uint32_t addr, uint8_t value);
    void Write16(uint32_t addr, uint16_t value);
    void Write32(uint32_t addr, uint32_t value);

    // Блоковые операции
    void WriteBytes(uint32_t addr, const void* data, size_t size);
    void Memset(uint32_t addr, uint8_t value, size_t size);

    // Получение указателя на память
    const uint8_t* GetPointer(uint32_t addr) const;
    uint8_t* GetPointer(uint32_t addr);

    // Размер памяти
    size_t GetSize() const { return ramSize_; }

private:
    std::unique_ptr<uint8_t[]> ram_;
    size_t ramSize_ = 0;

    void CheckBounds(uint32_t addr, size_t size) const;
    bool IsAligned(uint32_t addr, size_t alignment) const;
    void InitializeMemory();
};

} // namespace core
} // namespace ppsspp
