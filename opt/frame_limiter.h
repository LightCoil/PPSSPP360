#pragma once

#include <chrono>
#include <thread>

namespace ppsspp {
namespace opt {

class FrameLimiter {
public:
    explicit FrameLimiter(int targetFPS);
    void StartFrame();
    void EndFrame();

private:
    std::chrono::microseconds targetFrameTime_;
    std::chrono::time_point<std::chrono::high_resolution_clock> frameStart_;
};

} // namespace opt
} // namespace ppsspp 