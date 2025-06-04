#include "audio_output.h"
#include <cassert>
#include <cstdio>

// Конструктор
AudioOutput::AudioOutput()
  : xaudio2_(nullptr),
    sourceVoice_(nullptr),
    masteringVoice_(nullptr),
    sampleRate_(0),
    channelCount_(0) {
}

// Деструктор
AudioOutput::~AudioOutput() {
    Shutdown();
}

bool AudioOutput::Initialize(uint32_t sampleRate, uint32_t channels) {
    sampleRate_   = sampleRate;
    channelCount_ = channels;

    // Создаём XAudio2-инстанс
    HRESULT hr = XAudio2Create(&xaudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr) || xaudio2_ == nullptr) {
        return false;
    }

    // Создаём Mastering Voice
    hr = xaudio2_->CreateMasteringVoice(
        &masteringVoice_,
        channelCount_,
        sampleRate_,
        0,
        nullptr  // Для Xenon необязательно указывать конкретное устройство
    );
    if (FAILED(hr) || masteringVoice_ == nullptr) {
        xaudio2_->Release();
        xaudio2_ = nullptr;
        return false;
    }

    // Создаём Source Voice с форматом PCM16
    WAVEFORMATEX wfx = {};
    wfx.wFormatTag      = WAVE_FORMAT_PCM;
    wfx.nChannels       = static_cast<WORD>(channelCount_);
    wfx.nSamplesPerSec  = sampleRate_;
    wfx.wBitsPerSample  = 16;  // PCM16
    wfx.nBlockAlign     = wfx.nChannels * (wfx.wBitsPerSample / 8);
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    hr = xaudio2_->CreateSourceVoice(
        &sourceVoice_,
        &wfx,
        0,                         // флаги
        XAUDIO2_DEFAULT_FREQ_RATIO,
        nullptr,                   // Callback (можно nullptr)
        nullptr,                   // Кингблок — NOT_USED
        nullptr
    );
    if (FAILED(hr) || sourceVoice_ == nullptr) {
        masteringVoice_->DestroyVoice();
        xaudio2_->Release();
        xaudio2_ = nullptr;
        return false;
    }

    // Запускаем Source Voice
    hr = sourceVoice_->Start(0);
    if (FAILED(hr)) {
        sourceVoice_->DestroyVoice();
        masteringVoice_->DestroyVoice();
        xaudio2_->Release();
        xaudio2_ = nullptr;
        return false;
    }

    return true;
}

void AudioOutput::Shutdown() {
    if (sourceVoice_) {
        sourceVoice_->Stop(0);
        sourceVoice_->DestroyVoice();
        sourceVoice_ = nullptr;
    }
    if (masteringVoice_) {
        masteringVoice_->DestroyVoice();
        masteringVoice_ = nullptr;
    }
    if (xaudio2_) {
        xaudio2_->StopEngine();
        xaudio2_->Release();
        xaudio2_ = nullptr;
    }
}

bool AudioOutput::SubmitBuffer(const BYTE* pcmData, size_t length) {
    if (!sourceVoice_ || !pcmData || length == 0) {
        return false;
    }

    XAUDIO2_BUFFER buffer = {};
    buffer.pAudioData       = pcmData;
    buffer.AudioBytes      = static_cast<UINT32>(length);
    buffer.Flags           = XAUDIO2_END_OF_STREAM;
    buffer.PlayBegin       = 0;
    buffer.PlayLength      = 0;  // весь буфер
    buffer.LoopCount       = 0;  // один раз

    HRESULT hr = sourceVoice_->SubmitSourceBuffer(&buffer);
    return SUCCEEDED(hr);
}
