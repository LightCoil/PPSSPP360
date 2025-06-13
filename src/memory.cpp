#include "../xbox360/xbox360.hpp"
#include <windows.h>

namespace Xbox360 {

class Memory {
public:
    static bool Read(xbox360::u32 address, void* buffer, size_t size) {
    if (!buffer || size == 0) return false;
    
    SIZE_T bytesRead;
    return ReadProcessMemory(GetCurrentProcess(), 
                           reinterpret_cast<LPCVOID>(static_cast<uintptr_t>(address)),
                           buffer,
                           size,
                           &bytesRead) && bytesRead == size;
}

static bool Write(xbox360::u32 address, const void* buffer, size_t size) {
    if (!buffer || size == 0) return false;
    
    SIZE_T bytesWritten;
    return WriteProcessMemory(GetCurrentProcess(),
                            reinterpret_cast<LPVOID>(static_cast<uintptr_t>(address)),
                            buffer,
                            size,
                            &bytesWritten) && bytesWritten == size;
}

static bool Protect(xbox360::u32 address, size_t size, bool read, bool write, bool execute) {
    DWORD oldProtect;
    DWORD newProtect = 0;
    
    if (read && write && execute) newProtect = PAGE_EXECUTE_READWRITE;
    else if (read && write) newProtect = PAGE_READWRITE;
    else if (read && execute) newProtect = PAGE_EXECUTE_READ;
    else if (read) newProtect = PAGE_READONLY;
    else if (execute) newProtect = PAGE_EXECUTE;
    else newProtect = PAGE_NOACCESS;
    
    return VirtualProtect(reinterpret_cast<LPVOID>(static_cast<uintptr_t>(address)),
                         size,
                         newProtect,
                         &oldProtect) != 0;
}

};

} // namespace Xbox360
