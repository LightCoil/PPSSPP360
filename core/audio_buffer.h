#pragma once
#include <vector>
#include <cstdint>

namespace core {

struct AudioBuffer {
    std::vector<uint8_t> data;
    uint32_t size;
    bool isPlaying;
    float volume;
};

} // namespace core 