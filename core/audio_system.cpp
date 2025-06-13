#pragma warning(disable : 4005) // Отключаем предупреждение о переопределении макросов
#pragma warning(disable : 4273) // Отключаем предупреждение о несовместимом связывании DLL для GetTickCount
#include <windows.h>
#include <mmsystem.h>
#include "audio_system.h"
#include "audio_buffer.h"
#include "audio_voice.h"
#include "audio_utils.h"
#include <stdexcept>
#include <algorithm>
#include <mutex>
#include <memory>
#include <ranges>

namespace core {

AudioSystem& AudioSystem::GetInstance() {
    static AudioSystem instance;
    return instance;
}

bool AudioSystem::Initialize() {
    std::lock_guard lock(mutex_);
    
    if (!InitializeXAudio2()) {
        return false;
    }

    volume_ = 1.0f;
    return true;
}

void AudioSystem::Shutdown() {
    std::lock_guard lock(mutex_);

    // Stop and destroy all buffers
    for (auto* buffer : buffers_) {
        StopBuffer(buffer);
        DestroyBuffer(buffer);
    }
    buffers_.clear();

    // Destroy all source voices
    for (auto* voice : sourceVoices_) {
        if (voice) {
            voice->Stop();
            xbox360::DestroyXAudioSourceVoice(voice);
        }
    }
    sourceVoices_.clear();

    // Destroy mastering voice and XAudio2
    if (masteringVoice_) {
        xbox360::DestroyXAudioMasteringVoice(masteringVoice_);
        masteringVoice_ = nullptr;
    }

    if (xaudio2_) {
        xbox360::XAudioRelease(xaudio2_);
        xaudio2_ = nullptr;
    }
}

void AudioSystem::Update() {
    std::lock_guard lock(mutex_);

    // Update all playing buffers
    for (auto* buffer : buffers_) {
        if (buffer->isPlaying) {
            // TODO: Implement proper playback status check
            // For now, we'll just keep the buffer marked as playing
            // until it's explicitly stopped
        }
    }
}

void AudioSystem::SetVolume(float volume) {
    std::lock_guard lock(mutex_);
    volume_ = std::clamp(volume, 0.0f, 1.0f);

    if (masteringVoice_) {
        masteringVoice_->SetVolume(volume_);
    }
}

float AudioSystem::GetVolume() const {
    std::lock_guard lock(mutex_);
    return volume_;
}

std::unique_ptr<AudioBuffer> AudioSystem::CreateBuffer(std::span<const uint8_t> data) {
    std::lock_guard lock(mutex_);

    if (data.empty()) {
        return nullptr;
    }

    auto buffer = std::make_unique<AudioBuffer>();
    buffer->data.assign(data.begin(), data.end());
    buffer->size = static_cast<uint32_t>(data.size());
    buffer->isPlaying = false;
    buffer->volume = 1.0f;

    buffers_.push_back(buffer.get());
    return buffer;
}

void AudioSystem::DestroyBuffer(AudioBuffer* buffer) {
    std::lock_guard lock(mutex_);

    if (!buffer) {
        return;
    }

    // Останавливаем воспроизведение, если буфер играет
    if (buffer->isPlaying) {
        StopBuffer(buffer);
    }

    // Find and remove the buffer from our list
    if (auto it = std::find(buffers_.begin(), buffers_.end(), buffer); it != buffers_.end()) {
        buffers_.erase(it);
    }
}

void AudioSystem::PlayBuffer(AudioBuffer* buffer) {
    std::lock_guard lock(mutex_);

    if (!buffer || !xaudio2_) {
        return;
    }

    // Останавливаем предыдущее воспроизведение этого буфера
    if (buffer->isPlaying) {
        StopBuffer(buffer);
    }

    // Create a new source voice if needed
    AudioVoice* sourceVoice = nullptr;
    if (sourceVoices_.empty()) {
        WAVEFORMATEX format = {};
        format.wFormatTag = WAVE_FORMAT_PCM;
        format.nChannels = AUDIO_CHANNELS;
        format.nSamplesPerSec = AUDIO_SAMPLE_RATE;
        format.wBitsPerSample = AUDIO_BITS_PER_SAMPLE;
        format.nBlockAlign = (AUDIO_CHANNELS * AUDIO_BITS_PER_SAMPLE) / 8;
        format.nAvgBytesPerSec = AUDIO_SAMPLE_RATE * format.nBlockAlign;

        if (HRESULT result = xbox360::CreateXAudioSourceVoice(xaudio2_, &sourceVoice, reinterpret_cast<const xbox360::WAVEFORMATEX*>(&format)); result < 0) {
            return;
        }

        sourceVoices_.push_back(sourceVoice);
    } else {
        sourceVoice = sourceVoices_.back();
    }

    // Set up the audio buffer
    xbox360::XBOX_XAUDIO2_BUFFER xaudioBuffer;
    AUDIO_ZERO_MEMORY(&xaudioBuffer, sizeof(xaudioBuffer));
    xaudioBuffer.pAudioData = buffer->data.data();
    xaudioBuffer.AudioBytes = buffer->size;
    xaudioBuffer.PlayBegin = 0;
    xaudioBuffer.PlayLength = buffer->size;
    xaudioBuffer.LoopBegin = 0;
    xaudioBuffer.LoopLength = 0;
    xaudioBuffer.LoopCount = 0;
    xaudioBuffer.pContext = buffer;

    // Submit the buffer and start playing
    if (HRESULT result = xbox360::SubmitXAudioSourceBuffer(sourceVoice, &xaudioBuffer); result < 0) {
        return;
    }
    
    xbox360::SetXAudioVolume(sourceVoice, buffer->volume * volume_);
    xbox360::StartXAudioSourceVoice(sourceVoice);
    buffer->isPlaying = true;
}

void AudioSystem::StopBuffer(AudioBuffer* buffer) {
    std::lock_guard lock(mutex_);

    if (!buffer) {
        return;
    }

    // Find the source voice that's playing this buffer
    for (auto* voice : sourceVoices_) {
        if (voice) {
            voice->Stop();
        }
    }
    buffer->isPlaying = false;
}

bool AudioSystem::InitializeXAudio2() {
    if (HRESULT result = xbox360::XAudio2Create(&xaudio2_, 0, 0); result < 0) {
        return false;
    }

    if (HRESULT result = xbox360::CreateXAudioMasteringVoice(xaudio2_, &masteringVoice_); result < 0) {
        xbox360::XAudioRelease(xaudio2_);
        xaudio2_ = nullptr;
        return false;
    }

    return true;
}

void AudioSystem::ShutdownXAudio2() {
    if (masteringVoice_) {
        xbox360::DestroyXAudioMasteringVoice(masteringVoice_);
        masteringVoice_ = nullptr;
    }

    if (xaudio2_) {
        xbox360::XAudioRelease(xaudio2_);
        xaudio2_ = nullptr;
    }
}

void AudioSystem::SubmitAudio(const int16_t* pcm, uint32_t samples) {
    if (!pcm || samples == 0) return;
    std::span<const uint8_t> data(reinterpret_cast<const uint8_t*>(pcm), samples * AUDIO_CHANNELS * sizeof(int16_t));
    auto buffer = CreateBuffer(data);
    if (buffer) PlayBuffer(buffer.get());
}

} // namespace core 