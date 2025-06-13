#ifndef AUDIO_H
#define AUDIO_H

#include <cstdint>
#include <vector>
#include <memory>
#include <mutex>
#include "../xbox360/xaudio2.hpp"

namespace core {

// Audio constants
constexpr uint32_t AUDIO_SAMPLE_RATE = 48000;
constexpr uint16_t AUDIO_CHANNELS = 2;
constexpr uint16_t AUDIO_BITS_PER_SAMPLE = 16;
constexpr uint32_t AUDIO_BUFFER_SIZE = 4096;

class AudioSystem {
public:
    static AudioSystem& GetInstance();

    bool Initialize();
    void Shutdown();
    void Update();

    // Volume control
    void SetVolume(float volume);
    float GetVolume() const;

    // Audio buffer management
    struct AudioBuffer {
        std::vector<uint8_t> data;
        uint32_t size;
        bool isPlaying;
        float volume;
    };

    AudioBuffer* CreateBuffer(const uint8_t* data, uint32_t size);
    void DestroyBuffer(AudioBuffer* buffer);
    void PlayBuffer(AudioBuffer* buffer);
    void StopBuffer(AudioBuffer* buffer);

private:
    AudioSystem() = default;
    ~AudioSystem() = default;
    AudioSystem(const AudioSystem&) = delete;
    AudioSystem& operator=(const AudioSystem&) = delete;

    bool InitializeXAudio2();
    void ShutdownXAudio2();

    xbox360::XBOX_IXAudio2* xaudio2_;
    xbox360::XBOX_IXAudio2MasteringVoice* masteringVoice_;
    std::vector<xbox360::XBOX_IXAudio2SourceVoice*> sourceVoices_;
    std::vector<AudioBuffer*> buffers_;
    float volume_;
    mutable std::mutex mutex_;
};

} // namespace core

#endif // AUDIO_H
