#include "core/cpu.h"
#include "core/loader.h"
#include "video/d3d9_renderer.h"
#include "audio/xaudio.h"
#include "input/xinput.h"

int main() {
    Loader::LoadISO("content/game.iso");
    D3D9Renderer::Init();
    XAudio::Init();
    XInput::Init();

    while (true) {
        CPU::Step();
        D3D9Renderer::Flip();
    }

    return 0;
}
