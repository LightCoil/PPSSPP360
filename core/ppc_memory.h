#ifndef PPC_MEMORY_H
#define PPC_MEMORY_H

#include <cstdint>
#include <vector>

// Простая «обертка» над памятью PSP (32 МБ)
class PPCMemory {
public:
    PPCMemory();
    ~PPCMemory();

    bool Allocate(size_t size);
    void Free();

    // Чтение/запись в память по адресам PSP
    uint32_t ReadU32(uint32_t addr) const;
    void     WriteU32(uint32_t addr, uint32_t value);

    uint16_t ReadU16(uint32_t addr) const;
    void     WriteU16(uint32_t addr, uint16_t value);

    uint8_t  ReadU8(uint32_t addr) const;
    void     WriteU8(uint32_t addr, uint8_t value);

    // Прямой доступ к «сырым» байтам
    uint8_t* BasePtr()       { return memory_.data(); }
    const uint8_t* BasePtr() const { return memory_.data(); }

private:
    std::vector<uint8_t> memory_;
};

#endif // PPC_MEMORY_H
