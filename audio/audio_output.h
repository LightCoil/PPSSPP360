#ifndef AUDIO_OUTPUT_H
#define AUDIO_OUTPUT_H

#include <stdint.h>   // для uint8_t, uint32_t и т.п.
#include <XAudio2.h>  // Portable XAudio2 (libxenon предоставляет этот заголовок)
#include <xboxkrnl/xboxkrnl.h>  // для типовых Xbox-определений (LPVOID и т.п.)

// Если libxenon не содержит точно такого пути, можно использовать <xaudio2p.h>:
// #include <xaudio2p.h>

typedef uint8_t BYTE;  // На всякий случай, если BYTE не определён

class AudioOutput {
public:
    AudioOutput();
    ~AudioOutput();

    bool Initialize(uint32_t sampleRate, uint32_t channels);
    void   Shutdown();

    // Передаёт буфер с PCM-данными на воспроизведение
    bool SubmitBuffer(const BYTE* pcmData, size_t length);

private:
    IXAudio2*            xaudio2_;
    IXAudio2SourceVoice* sourceVoice_;
    IXAudio2MasteringVoice* masteringVoice_;

    uint32_t             sampleRate_;
    uint32_t             channelCount_;
};

#endif // AUDIO_OUTPUT_H
