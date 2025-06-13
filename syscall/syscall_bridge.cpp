// syscall/syscall_bridge.cpp
#include "syscall_handler.h"

// Глобальный указатель, выставляется в main()
ppsspp::syscall::SyscallHandler* g_syscallHandler = nullptr;

// Экспортируемая точка входа из JIT
extern "C" uint32_t HandleSyscall(uint32_t syscallID) {
    return g_syscallHandler->Invoke(syscallID);
}
