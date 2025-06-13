#include "syscall_handler.h"
#include "../core/cpu_state.h"
#include "../core/memory.h"
#include "../core/audio_system.h"
#include "../video/video_engine.h"
#include <memory>

namespace ppsspp {
namespace syscall {

// Глобальные экземпляры
static std::unique_ptr<SyscallHandler> g_syscallHandler;

void HandleSyscall(core::CPUState* cpu, uint32_t syscallID) {
    if (!cpu) return;

    // Создаем обработчик при первом вызове
    if (!g_syscallHandler) {
        auto* memory = cpu->GetMemory();
        if (!memory) return;

    // Создаем временные объекты для аудио и видео с параметрами по умолчанию
    static auto& audio = core::AudioSystem::GetInstance();
    static video::VideoEngine video(480, 272); // Разрешение PSP экрана

        g_syscallHandler = std::make_unique<SyscallHandler>(*cpu, *memory, audio, video);
    }

    // Вызываем обработчик
    uint32_t result = g_syscallHandler->Invoke(syscallID);
    cpu->SetPC(result);
}

}  // namespace syscall
}  // namespace ppsspp
