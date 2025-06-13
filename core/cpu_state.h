// core/cpu_state.h

#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <memory>

namespace ppsspp {
namespace core {

class CPUError : public std::runtime_error {
public:
    explicit CPUError(const std::string& message) : std::runtime_error(message) {}
    virtual ~CPUError() = default;
};

class Memory;

class CPUState {
public:
    static constexpr uint32_t MAX_MEMORY = 0x10000000; // 256MB
    static constexpr uint32_t INVALID_PC = 0xFFFFFFFF;
    static constexpr size_t VFPU_REG_COUNT = 128;      // Количество регистров VFPU
    static constexpr size_t VFPU_VECTOR_SIZE = 4;      // Размер вектора VFPU

    CPUState();                             // Конструктор по умолчанию
    CPUState(uint32_t pc, uint32_t gp);     // Конструктор с инициализацией
    ~CPUState() = default;                  // Деструктор

    // Запрещаем копирование
    CPUState(const CPUState&) = delete;
    CPUState& operator=(const CPUState&) = delete;

    // Разрешаем перемещение
    CPUState(CPUState&&) noexcept = default;
    CPUState& operator=(CPUState&&) noexcept = default;

    void Reset();                           // Сброс в 0
    void Reset(uint32_t pc, uint32_t gp);   // Сброс с установкой PC и GP

    // Безопасные методы доступа к регистрам
    uint32_t GetGPR(size_t index) const;
    void SetGPR(size_t index, uint32_t value);
    
    // Безопасные методы для специальных регистров
    uint32_t GetPC() const { return pc; }
    void SetPC(uint32_t new_pc);
    
    uint32_t GetGP() const { return gp; }
    void SetGP(uint32_t new_gp);
    
    uint32_t GetLO() const { return lo; }
    void SetLO(uint32_t new_lo);
    
    uint32_t GetHI() const { return hi; }
    void SetHI(uint32_t new_hi);

    // Методы для работы с VFPU
    float GetVFPU(size_t reg, size_t index) const;
    void SetVFPU(size_t reg, size_t index, float value);
    const float* GetVFPUVector(size_t reg) const;
    void SetVFPUVector(size_t reg, const float* values);

    // Методы для работы с scratch регистрами
    uint32_t GetScratchPC() const { return scratchPC; }
    void SetScratchPC(uint32_t new_pc) { scratchPC = new_pc; }
    uint32_t GetScratchInst() const { return scratchInst; }
    void SetScratchInst(uint32_t new_inst) { scratchInst = new_inst; }

    // Установка указателя на память
    void SetMemory(Memory* mem) { memory = mem; }
    Memory* GetMemory() const { return memory; }

    // Методы для JIT
    uint32_t* GetGPRPtr() { return gpr; }
    uint32_t* GetLOPtr() { return &lo; }
    uint32_t* GetHIPtr() { return &hi; }
    uint32_t* GetPCPtr() { return &pc; }
    uint32_t* GetScratchPCPtr() { return &scratchPC; }
    uint32_t* GetScratchInstPtr() { return &scratchInst; }

private:
    uint32_t gpr[32] = {0};
    uint32_t lo = 0;
    uint32_t hi = 0;
    uint32_t pc = 0;
    uint32_t gp = 0;
    uint32_t scratchPC = 0;    // Временный PC для интерпретатора
    uint32_t scratchInst = 0;  // Текущая инструкция
    Memory* memory = nullptr;
    float vpr[VFPU_REG_COUNT][VFPU_VECTOR_SIZE] = {{0.0f}};  // Регистры VFPU

    void ValidateRegisterIndex(size_t index) const;
    void ValidatePC(uint32_t new_pc) const;
    void ValidateMemory() const;
    void ValidateVFPURegister(size_t reg) const;
    void ValidateVFPUIndex(size_t index) const;
};

}  // namespace core
}  // namespace ppsspp
