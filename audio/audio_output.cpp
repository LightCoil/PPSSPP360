#include "audio_output.h"
#include <xaudio2/xaudio2.h> // псевдокод, для Xbox360 может быть XAudio2

static IXAudio2 *xaudio = nullptr;
static IXAudio2SourceVoice *voice = nullptr;

void AudioOutput_Init() {
    XAudio2Create(&xaudio, 0, XAUDIO2_DEFAULT_PROCESSOR);
    xaudio->CreateMasteringVoice(&voice);
}

void AudioOutput_Shutdown() {
    if (voice) voice->DestroyVoice();
    if (xaudio) xaudio->Release();
}

void AudioOutput_PlayPCM(const int16_t *pcmData, int samples) {
    if (!voice) return;
    XAUDIO2_BUFFER buf = {0};
    buf.AudioBytes = samples * sizeof(int16_t) * 2; // стерео
    buf.pAudioData = (BYTE*)pcmData;
    buf.Flags = XAUDIO2_END_OF_STREAM;
    voice->SubmitSourceBuffer(&buf);
}
