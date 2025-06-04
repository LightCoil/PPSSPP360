#ifndef AUDIO_OUTPUT_H
#define AUDIO_OUTPUT_H

#include <stdint.h>
#include <xaudio2/xaudio2.h>  // Используем правильный путь для libxenon
#include <xboxkrnl/xboxkrnl.h>

typedef uint8_t BYTE;

class AudioOutput {
public:
    AudioOutput();
    ~AudioOutput();

    bool Initialize(uint32_t sampleRate, uint32_t channels);
    void Shutdown();

    bool SubmitBuffer(const BYTE* pcmData, size_t length);

private:
    IXAudio2* xaudio2_;
    IXAudio2SourceVoice* sourceVoice_;
    IXAudio2MasteringVoice* masteringVoice_;

    uint32_t sampleRate_;
    uint32_t channelCount_;
};

#endif // AUDIO_OUTPUT_H
