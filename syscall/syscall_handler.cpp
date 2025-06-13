// syscall/syscall_handler.cpp

#include "syscall_handler.h"
#include <thread>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <filesystem>
#include <fstream>

namespace ppsspp {
namespace syscall {

using namespace std::chrono;

SyscallHandler::SyscallHandler(core::CPUState& cpu, core::Memory& memory,
                               core::AudioSystem& audio, video::VideoEngine& video)
    : cpu_(cpu), memory_(memory), audio_(audio), video_(video), startTime_(steady_clock::now()) {
    std::filesystem::create_directories("saves");
}

uint32_t SyscallHandler::Invoke(uint32_t syscallID) {
    switch (syscallID) {
        case 0x20: Sys_DisplayWaitVblankStart(); break;
        case 0x21: Sys_DisplaySetMode(); break;
        case 0x30: Sys_CtrlReadBufferPositive(); break;
        case 0x31: Sys_CtrlPeekBufferPositive(); break;
        case 0x10: Sys_ExitGame(); break;
        case 0x40: Sys_RtcGetTick(); break;
        case 0x50: Sys_AudioOutput(); break;
        case 0x70: Sys_UtilitySavedata(); break;
        case 0xA0: Sys_IoOpen(); break;
        case 0xA1: Sys_IoRead(); break;
        case 0xA2: Sys_IoWrite(); break;
        case 0xA3: Sys_IoClose(); break;
        default:
            std::fprintf(stderr, "[SyscallHandler] Unknown syscall 0x%X\n", syscallID);
            writeResult(uint32_t(-1));
            break;
    }
    return cpu_.GetPC();
}

void SyscallHandler::writeResult(uint32_t value) {
    cpu_.SetGPR(2, value);
}

void SyscallHandler::Sys_DisplayWaitVblankStart() {
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
    writeResult(0);
}

void SyscallHandler::Sys_DisplaySetMode() {
    uint32_t w = cpu_.GetGPR(5), h = cpu_.GetGPR(6);
    video_ = video::VideoEngine(w, h);
    writeResult(0);
}

void SyscallHandler::Sys_CtrlReadBufferPositive() {
    uint32_t addr = cpu_.GetGPR(4);
    uint32_t val = memory_.Read32(0x88000000);
    uint8_t lx = memory_.Read8(0x88000004);
    uint8_t ly = memory_.Read8(0x88000005);

    memory_.Write32(addr, val);
    memory_.Write8(addr + 4, lx);
    memory_.Write8(addr + 5, ly);
    for (int i = 0; i < 6; ++i)
        memory_.Write8(addr + 6 + i, 0);

    writeResult(1);
}

void SyscallHandler::Sys_CtrlPeekBufferPositive() {
    Sys_CtrlReadBufferPositive();
}

void SyscallHandler::Sys_ExitGame() {
    std::exit(0);
}

void SyscallHandler::Sys_RtcGetTick() {
    auto elapsed = duration_cast<microseconds>(steady_clock::now() - startTime_);
    writeResult(static_cast<uint32_t>(elapsed.count()));
}

void SyscallHandler::Sys_AudioOutput() {
    uint32_t pcmAddr = cpu_.GetGPR(4);
    uint32_t samples = cpu_.GetGPR(5);
    uint32_t total = samples * audio_.Channels();

    std::vector<int16_t> buf(total);
    for (uint32_t i = 0; i < total; ++i)
        buf[i] = static_cast<int16_t>(memory_.Read16(pcmAddr + i * 2));

    audio_.SubmitAudio(buf.data(), samples);
    writeResult(0);
}

void SyscallHandler::Sys_UtilitySavedata() {
    uint32_t bufAddr = cpu_.GetGPR(4);
    uint32_t size = cpu_.GetGPR(5);
    uint32_t titlePtr = cpu_.GetGPR(6);

    std::string title;
    for (int i = 0; i < 32; ++i) {
        char c = static_cast<char>(memory_.Read8(titlePtr + i));
        if (!c) break;
        title.push_back(c);
    }

    if (title.empty()) {
        writeResult(uint32_t(-1));
        return;
    }

    std::string path = "saves/" + title + ".sav";
    std::ofstream f(path, std::ios::binary);
    if (!f) {
        writeResult(uint32_t(-1));
        return;
    }

    for (uint32_t i = 0; i < size; ++i) {
        char c = static_cast<char>(memory_.Read8(bufAddr + i));
        f.put(c);
    }

    f.close();
    writeResult(0);
}

void SyscallHandler::Sys_IoOpen() {
    uint32_t pathPtr = cpu_.GetGPR(4);
    uint32_t flags = cpu_.GetGPR(5);
    uint32_t mode = cpu_.GetGPR(6);

    std::string path;
    for (int i = 0; i < 256; ++i) {
        char c = static_cast<char>(memory_.Read8(pathPtr + i));
        if (!c) break;
        path.push_back(c);
    }

    const char* modeStr = (flags & 0x00000100) ? "rb+" : "wb+";
    FILE* f = std::fopen(path.c_str(), modeStr);
    if (!f) f = std::fopen(path.c_str(), "rb+");

    if (!f) {
        writeResult(uint32_t(-1));
        return;
    }

    int fd = nextFd_++;
    fdMap_[fd] = f;
    writeResult(fd);
}

void SyscallHandler::Sys_IoRead() {
    int fd = static_cast<int>(cpu_.GetGPR(4));
    uint32_t bufPtr = cpu_.GetGPR(5);
    uint32_t size = cpu_.GetGPR(6);

    auto it = fdMap_.find(fd);
    if (it == fdMap_.end()) {
        writeResult(uint32_t(-1));
        return;
    }

    FILE* f = it->second;
    std::vector<char> temp(size);
    size_t read = std::fread(temp.data(), 1, size, f);

    for (size_t i = 0; i < read; ++i)
        memory_.Write8(bufPtr + static_cast<uint32_t>(i), static_cast<uint8_t>(temp[i]));

    writeResult(static_cast<uint32_t>(read));
}

void SyscallHandler::Sys_IoWrite() {
    int fd = static_cast<int>(cpu_.GetGPR(4));
    uint32_t bufPtr = cpu_.GetGPR(5);
    uint32_t size = cpu_.GetGPR(6);

    auto it = fdMap_.find(fd);
    if (it == fdMap_.end()) {
        writeResult(uint32_t(-1));
        return;
    }

    FILE* f = it->second;
    std::vector<char> temp(size);
    for (uint32_t i = 0; i < size; ++i)
        temp[i] = static_cast<char>(memory_.Read8(bufPtr + i));

    size_t written = std::fwrite(temp.data(), 1, size, f);
    writeResult(static_cast<uint32_t>(written));
}

void SyscallHandler::Sys_IoClose() {
    int fd = static_cast<int>(cpu_.GetGPR(4));

    auto it = fdMap_.find(fd);
    if (it != fdMap_.end()) {
        std::fclose(it->second);
        fdMap_.erase(it);
        writeResult(0);
    } else {
        writeResult(uint32_t(-1));
    }
}

}  // namespace syscall
}  // namespace ppsspp
