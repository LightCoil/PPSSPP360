#pragma once
#include <cstdint>
#include "../core/cpu_state.h"

namespace ppsspp {
namespace syscall {

enum SyscallID : uint32_t {
    SYSCALL_EXIT = 0x00000001,
    SYSCALL_GET_TIME = 0x00000002,
    SYSCALL_CTRL_INPUT = 0x00000003,
    // и т.п.
};

void HandleSyscall(core::CPUState* st, uint32_t syscallID);

}
}
