#include "psp_cpu.h"
#include "cpu.h"
#include "memory.h"
#include "../jit/jit_block_cache.h"
#include "../jit/jit_compiler.h"
#include "../jit/jit_runtime.h"
#include "../jit/jit_interrupt_bridge.h"

CPUState cpu;

static core::Memory *g_memory = nullptr;
static core::CPU *    g_cpu    = nullptr;
static JIT::JITBlockCache jitCache;
static JIT::JITCompiler *jitCompiler = nullptr;

void CPU_Init() {
    g_memory = new core::Memory(PSP_RAM_SIZE);
    g_cpu    = new core::CPU(*g_memory, cpu);
    cpu.PC   = 0;
    // Инициализируем JIT
    jitCompiler = new JIT::JITCompiler(jitCache, cpu);
    JIT::InitJitRuntime();
}

void CPU_Shutdown() {
    delete g_cpu;
    delete g_memory;
    delete jitCompiler;
    g_cpu = nullptr;
    g_memory = nullptr;
    jitCompiler = nullptr;
}

void CPU_Execute(int cycles) {
    if (!g_cpu) return;
    g_cpu->Run(cycles);
}

// Основной цикл: JIT + интерпретатор
void PSP_RunWithJIT(int cycles) {
    if (!jitCompiler) return;
    int executed = 0;
    while (executed < cycles && cpu.running) {
        JIT::JITBlock *block = jitCompiler->Compile(cpu.PC);
        if (block) {
            JIT::ExecuteJitBlock(block, &cpu);
            if (JIT::jitShouldExit) {
                g_cpu->Run(1);
                executed++;
                continue;
            }
            executed++;
        } else {
            g_cpu->Run(1);
            executed++;
        }
    }
}
