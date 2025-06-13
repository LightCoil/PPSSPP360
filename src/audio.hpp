#ifndef AUDIO_HPP
#define AUDIO_HPP

#include "../xbox360/xbox360.hpp"
#include <windows.h>
#include <mmsystem.h>
#include <memory>
#include <vector>

namespace Xbox360 {
using xbox360::u32;
using xbox360::u64;

// XAudio2 константы
constexpr float XAUDIO2_MAX_VOLUME_LEVEL = 2.0f;
constexpr float XAUDIO2_MIN_FREQUENCY_RATIO = 0.0001f;
constexpr float XAUDIO2_MAX_FREQUENCY_RATIO = 1024.0f;
constexpr float XAUDIO2_MIN_PAN = -1.0f;
constexpr float XAUDIO2_MAX_PAN = 1.0f;
constexpr u32 XAUDIO2_MAX_QUEUED_BUFFERS = 64;

// XAudio2 события
constexpr u32 XAUDIO2_VOICE_EVENT_BUFFER_START = 0;
constexpr u32 XAUDIO2_VOICE_EVENT_BUFFER_END = 1;
constexpr u32 XAUDIO2_VOICE_EVENT_STREAM_END = 2;
constexpr u32 XAUDIO2_VOICE_EVENT_LOOP_END = 3;

// XAudio2 структуры
struct XAUDIO2_VOICE_STATE {
    u32 buffersQueued;
    u64 samplesPlayed;
};

struct XAUDIO2_FILTER_PARAMETERS {
    float LowPassFilter;
    float HighPassFilter;
    float BandPassFilter;
};

struct XAUDIO2_3D_AUDIO_PARAMETERS {
    float Position[3];
    float Velocity[3];
    float Orientation[3];
};

// XAudio2 колбэки
typedef void (CALLBACK *XAUDIO2_BUFFER_CALLBACK)(void* pBufferContext);
typedef void (CALLBACK *XAUDIO2_VOICE_CALLBACK)(void* pVoiceContext, u32 event);

class AudioVoice {
public:
    virtual ~AudioVoice() = default;
    virtual void SetVolume(float Volume) = 0;
    virtual void SetFrequencyRatio(float Ratio) = 0;
    virtual void SetSampleRate(u32 newSampleRate) = 0;
    virtual void SetFilter(const XAUDIO2_FILTER_PARAMETERS* parameters) = 0;
    virtual void Set3DAudioParameters(const XAUDIO2_3D_AUDIO_PARAMETERS* params) = 0;
    virtual void SetOutputMatrix(const float* levelMatrix, u32 sourceChannels, u32 destinationChannels) = 0;
};

class Audio {
public:
    Audio();
    ~Audio();

    bool Initialize();
    void Shutdown();
    bool IsInitialized() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace Xbox360

#endif // AUDIO_HPP 