// syscall/syscall_handler.h

#pragma once

#include "../core/cpu_state.h"
#include "../core/memory.h"
#include "../core/audio_system.h"
#include "../video/video_engine.h"

#include <unordered_map>
#include <string>
#include <chrono>
#include <cstdint>
#include <cstdio>

namespace ppsspp {
namespace syscall {

class SyscallHandler {
public:
    SyscallHandler(core::CPUState& cpu, core::Memory& memory,
                   core::AudioSystem& audio, video::VideoEngine& video);

    uint32_t Invoke(uint32_t syscallID);

private:
    core::CPUState& cpu_;
    core::Memory& memory_;
    core::AudioSystem& audio_;
    video::VideoEngine& video_;

    std::chrono::steady_clock::time_point startTime_;

    // fd → FILE* mapping for sceIo*
    std::unordered_map<int, FILE*> fdMap_;
    int nextFd_ = 3;  // 0,1,2 зарезервированы

    void writeResult(uint32_t value);

    // Syscall implementations
    void Sys_DisplayWaitVblankStart();
    void Sys_DisplaySetMode();
    void Sys_CtrlReadBufferPositive();
    void Sys_CtrlPeekBufferPositive();
    void Sys_ExitGame();
    void Sys_RtcGetTick();
    void Sys_AudioOutput();
    void Sys_UtilitySavedata();
    void Sys_IoOpen();
    void Sys_IoRead();
    void Sys_IoWrite();
    void Sys_IoClose();
};

}  // namespace syscall
}  // namespace ppsspp
