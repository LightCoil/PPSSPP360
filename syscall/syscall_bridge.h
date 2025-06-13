#pragma once

#include <cstdint>

namespace ppsspp {
namespace syscall {

class SyscallHandler {
public:
    virtual ~SyscallHandler() = default;
    virtual uint32_t HandleSyscall(uint32_t syscallID) = 0;
};

extern SyscallHandler* g_syscallHandler;

}  // namespace syscall
}  // namespace ppsspp

extern "C" {
    uint32_t HandleSyscall(uint32_t syscallID);
}
