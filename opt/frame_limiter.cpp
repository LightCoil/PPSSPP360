#include "frame_limiter.h"
#include <thread>

namespace ppsspp {
namespace opt {

FrameLimiter::FrameLimiter(int targetFPS)
    : targetFrameTime_(std::chrono::microseconds(1000000 / targetFPS)) {
}

void FrameLimiter::StartFrame() {
    frameStart_ = std::chrono::high_resolution_clock::now();
}

void FrameLimiter::EndFrame() {
    auto frameEnd = std::chrono::high_resolution_clock::now();
    auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(
        frameEnd - frameStart_);
    
    if (frameTime < targetFrameTime_) {
        std::this_thread::sleep_for(targetFrameTime_ - frameTime);
    }
}

} // namespace opt
} // namespace ppsspp
