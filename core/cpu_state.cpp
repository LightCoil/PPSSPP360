// core/cpu_state.cpp

#include "cpu_state.h"
#include "memory.h"
#include "logger.h"
#include <cstring>
#include <stdexcept>

namespace ppsspp {
namespace core {

CPUState::CPUState() {
    Reset();
}

CPUState::CPUState(uint32_t pc, uint32_t gp) {
    Reset(pc, gp);
}

void CPUState::Reset() {
    std::memset(gpr, 0, sizeof(gpr));
    lo = 0;
    hi = 0;
    pc = 0;
    gp = 0;
    memory = nullptr;
    LogInfo("CPU state reset to default values");
}

void CPUState::Reset(uint32_t pc, uint32_t gp) {
    std::memset(gpr, 0, sizeof(gpr));
    lo = 0;
    hi = 0;
    SetPC(pc);
    SetGP(gp);
    memory = nullptr;
    LogInfo("CPU state reset with PC=" + std::to_string(pc) + 
            ", GP=" + std::to_string(gp));
}

void CPUState::ValidateRegisterIndex(size_t index) const {
    if (index >= 32) {
        throw CPUError("Invalid register index: " + std::to_string(index));
    }
}

void CPUState::ValidatePC(uint32_t new_pc) const {
    if (new_pc >= MAX_MEMORY) {
        throw CPUError("Invalid PC value: " + std::to_string(new_pc));
    }
}

void CPUState::ValidateMemory() const {
    if (!memory) {
        throw CPUError("Memory not initialized");
    }
}

void CPUState::ValidateVFPURegister(size_t reg) const {
    if (reg >= VFPU_REG_COUNT) {
        throw CPUError("Invalid VFPU register index: " + std::to_string(reg));
    }
}

void CPUState::ValidateVFPUIndex(size_t index) const {
    if (index >= VFPU_VECTOR_SIZE) {
        throw CPUError("Invalid VFPU vector index: " + std::to_string(index));
    }
}

uint32_t CPUState::GetGPR(size_t index) const {
    ValidateRegisterIndex(index);
    return gpr[index];
}

void CPUState::SetGPR(size_t index, uint32_t value) {
    ValidateRegisterIndex(index);
    gpr[index] = value;
}

void CPUState::SetPC(uint32_t new_pc) {
    ValidatePC(new_pc);
    pc = new_pc;
}

void CPUState::SetGP(uint32_t new_gp) {
    if (new_gp >= MAX_MEMORY) {
        throw CPUError("Invalid GP value: " + std::to_string(new_gp));
    }
    gp = new_gp;
}

void CPUState::SetLO(uint32_t new_lo) {
    lo = new_lo;
}

void CPUState::SetHI(uint32_t new_hi) {
    hi = new_hi;
}

float CPUState::GetVFPU(size_t reg, size_t index) const {
    ValidateVFPURegister(reg);
    ValidateVFPUIndex(index);
    return vpr[reg][index];
}

void CPUState::SetVFPU(size_t reg, size_t index, float value) {
    ValidateVFPURegister(reg);
    ValidateVFPUIndex(index);
    vpr[reg][index] = value;
}

const float* CPUState::GetVFPUVector(size_t reg) const {
    ValidateVFPURegister(reg);
    return vpr[reg];
}

void CPUState::SetVFPUVector(size_t reg, const float* values) {
    ValidateVFPURegister(reg);
    if (!values) {
        throw CPUError("Null pointer passed to SetVFPUVector");
    }
    for (size_t i = 0; i < VFPU_VECTOR_SIZE; ++i) {
        vpr[reg][i] = values[i];
    }
}

}  // namespace core
}  // namespace ppsspp
