#include "jit_runtime.h"
#include <cstdlib>
#include <cstdint>
#ifdef _WIN32
  #include <windows.h>
#else
  #include <sys/mman.h>
  #include <unistd.h>
#endif

namespace JIT {

static constexpr size_t JIT_CODE_AREA_SIZE = 8 * 1024 * 1024; // 8 MB
static uint8_t *jit_code_area = nullptr;
static size_t jit_code_offset = 0;

bool jitShouldExit = false;

void InitJitRuntime() {
    if (!jit_code_area) {
#ifdef _WIN32
        jit_code_area = (uint8_t*)VirtualAlloc(nullptr, JIT_CODE_AREA_SIZE,
                                               MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#else
        void *p = mmap(nullptr, JIT_CODE_AREA_SIZE,
                       PROT_READ | PROT_WRITE | PROT_EXEC,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        jit_code_area = (p == MAP_FAILED) ? nullptr : (uint8_t*)p;
#endif
        jit_code_offset = 0;
        if (!jit_code_area) {
            std::abort();
        }
    }
}

uint8_t *AllocateExecutableMemory(size_t size) {
    if (jit_code_offset + size > JIT_CODE_AREA_SIZE) {
        return nullptr;
    }
    uint8_t *dest = jit_code_area + jit_code_offset;
    jit_code_offset += (size + 15) & ~15; // выравнивание по 16 байт
    return dest;
}

void ResetJitRuntime() {
    jit_code_offset = 0;
    jitShouldExit = false;
}

void SignalExitToInterpreter() {
    jitShouldExit = true;
}

} // namespace JIT
