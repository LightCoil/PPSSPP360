#pragma once
#include <vector>
#include <mutex>
#include <memory>
#include <span>
#include "audio_buffer.h"
#include "audio_voice.h"

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
    void SetVolume(float volume);
    float GetVolume() const;

    std::unique_ptr<AudioBuffer> CreateBuffer(std::span<const uint8_t> data);
    void DestroyBuffer(AudioBuffer* buffer);
    void PlayBuffer(AudioBuffer* buffer);
    void StopBuffer(AudioBuffer* buffer);

    // Новый метод для отправки PCM-данных (16-бит, signed, interleaved)
    void SubmitAudio(const int16_t* pcm, uint32_t samples);
    // Получить количество каналов
    uint32_t Channels() const { return AUDIO_CHANNELS; }

private:
    AudioSystem() = default;
    ~AudioSystem() = default;
    AudioSystem(const AudioSystem&) = delete;
    AudioSystem& operator=(const AudioSystem&) = delete;

    bool InitializeXAudio2();
    void ShutdownXAudio2();

    xbox360::XBOX_IXAudio2* xaudio2_{nullptr};
    AudioMasteringVoice* masteringVoice_{nullptr};
    std::vector<AudioVoice*> sourceVoices_;
    std::vector<AudioBuffer*> buffers_;
    float volume_{1.0f};
    mutable std::mutex mutex_;
};

} // namespace core 