#include "xaudio2.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <future>
#include <chrono>
#include <algorithm>

namespace {
    // Глобальные переменные для хранения состояния
    std::unordered_map<void*, std::unique_ptr<XBOX_IXAudio2>> xaudio2_instances;
    std::unordered_map<void*, std::unique_ptr<XBOX_IXAudio2MasteringVoice>> mastering_voice_instances;
    std::unordered_map<void*, std::unique_ptr<XBOX_IXAudio2SourceVoice>> source_voice_instances;
    std::mutex xaudio2_mutex;
    std::mutex mastering_voice_mutex;
    std::mutex source_voice_mutex;
}

namespace xbox360 {

XBOX_IXAudio2::XBOX_IXAudio2(void* xaudio2) : m_xaudio2(xaudio2) {
    std::lock_guard<std::mutex> lock(xaudio2_mutex);
    xaudio2_instances[xaudio2] = std::unique_ptr<XBOX_IXAudio2>(this);
}

XBOX_IXAudio2::~XBOX_IXAudio2() {
    std::lock_guard<std::mutex> lock(xaudio2_mutex);
    xaudio2_instances.erase(m_xaudio2);
    if (m_xaudio2) {
        // Освобождаем ресурсы
        ClearSourceVoices();
    }
}

u32 XBOX_IXAudio2::CreateMasteringVoice(XBOX_IXAudio2MasteringVoice** ppMasteringVoice,
    u32 InputChannels, u32 InputSampleRate, u32 Flags, u32 DeviceIndex,
    const XAUDIO2_EFFECT_CHAIN* pEffectChain) {
    void* voice = nullptr;
    u32 hr = static_cast<IXAudio2*>(m_xaudio2)->CreateMasteringVoice(&voice, InputChannels, InputSampleRate,
        Flags, DeviceIndex, pEffectChain);
    if (hr != 0) return hr;

    std::lock_guard<std::mutex> lock(mastering_voice_mutex);
    *ppMasteringVoice = new XBOX_IXAudio2MasteringVoice(voice);
    mastering_voice_instances[voice] = std::unique_ptr<XBOX_IXAudio2MasteringVoice>(*ppMasteringVoice);
    return 0;
}

u32 XBOX_IXAudio2::CreateSourceVoice(XBOX_IXAudio2SourceVoice** ppSourceVoice,
    const WAVEFORMATEX* pSourceFormat, u32 Flags, f32 MaxFrequencyRatio,
    void* pCallback, const XAUDIO2_VOICE_SENDS* pSendList,
    const XAUDIO2_EFFECT_CHAIN* pEffectChain) {
    void* voice = nullptr;
    u32 hr = static_cast<IXAudio2*>(m_xaudio2)->CreateSourceVoice(&voice, pSourceFormat, Flags,
        MaxFrequencyRatio, pCallback, pSendList, pEffectChain);
    if (hr != 0) return hr;

    std::lock_guard<std::mutex> lock(source_voice_mutex);
    *ppSourceVoice = new XBOX_IXAudio2SourceVoice(voice);
    source_voice_instances[voice] = std::unique_ptr<XBOX_IXAudio2SourceVoice>(*ppSourceVoice);
    return 0;
}

void XBOX_IXAudio2::ClearSourceVoices() {
    std::lock_guard<std::mutex> lock(source_voice_mutex);
    source_voice_instances.clear();
}

u32 XBOX_IXAudio2::GetSourceVoiceCount() const {
    std::lock_guard<std::mutex> lock(source_voice_mutex);
    return static_cast<u32>(source_voice_instances.size());
}

XBOX_IXAudio2MasteringVoice::XBOX_IXAudio2MasteringVoice(void* voice) : m_voice(voice), m_volume(1.0f) {
    std::lock_guard<std::mutex> lock(mastering_voice_mutex);
    mastering_voice_instances[voice] = std::unique_ptr<XBOX_IXAudio2MasteringVoice>(this);
}

XBOX_IXAudio2MasteringVoice::~XBOX_IXAudio2MasteringVoice() {
    std::lock_guard<std::mutex> lock(mastering_voice_mutex);
    mastering_voice_instances.erase(m_voice);
    if (m_voice) {
        static_cast<IXAudio2MasteringVoice*>(m_voice)->DestroyVoice();
    }
}

u32 XBOX_IXAudio2MasteringVoice::GetVolume(f32* pVolume) {
    if (!pVolume) return 1;
    *pVolume = m_volume;
    return 0;
}

u32 XBOX_IXAudio2MasteringVoice::SetVolume(f32 Volume, u32 OperationSet) {
    m_volume = Volume;
    return static_cast<IXAudio2MasteringVoice*>(m_voice)->SetVolume(Volume, OperationSet);
}

XBOX_IXAudio2SourceVoice::XBOX_IXAudio2SourceVoice(void* voice) : m_voice(voice), m_volume(1.0f), m_is_playing(false) {
    std::lock_guard<std::mutex> lock(source_voice_mutex);
    source_voice_instances[voice] = std::unique_ptr<XBOX_IXAudio2SourceVoice>(this);
}

XBOX_IXAudio2SourceVoice::~XBOX_IXAudio2SourceVoice() {
    std::lock_guard<std::mutex> lock(source_voice_mutex);
    source_voice_instances.erase(m_voice);
    if (m_voice) {
        static_cast<IXAudio2SourceVoice*>(m_voice)->DestroyVoice();
    }
}

u32 XBOX_IXAudio2SourceVoice::Start(u32 Flags, u32 OperationSet) {
    m_is_playing = true;
    return static_cast<IXAudio2SourceVoice*>(m_voice)->Start(Flags, OperationSet);
}

u32 XBOX_IXAudio2SourceVoice::Stop(u32 Flags, u32 OperationSet) {
    m_is_playing = false;
    return static_cast<IXAudio2SourceVoice*>(m_voice)->Stop(Flags, OperationSet);
}

u32 XBOX_IXAudio2SourceVoice::SubmitSourceBuffer(const XAUDIO2_BUFFER* pBuffer,
    const XAUDIO2_BUFFER_WMA* pBufferWMA) {
    if (pBuffer) {
        m_buffers.push_back(*pBuffer);
    }
    return static_cast<IXAudio2SourceVoice*>(m_voice)->SubmitSourceBuffer(pBuffer, pBufferWMA);
}

u32 XBOX_IXAudio2SourceVoice::FlushSourceBuffers() {
    m_buffers.clear();
    return static_cast<IXAudio2SourceVoice*>(m_voice)->FlushSourceBuffers();
}

u32 XBOX_IXAudio2SourceVoice::GetVolume(f32* pVolume) {
    if (!pVolume) return 1;
    *pVolume = m_volume;
    return 0;
}

u32 XBOX_IXAudio2SourceVoice::SetVolume(f32 Volume, u32 OperationSet) {
    m_volume = Volume;
    return static_cast<IXAudio2SourceVoice*>(m_voice)->SetVolume(Volume, OperationSet);
}

// XAudio2 functions
u32 XAudio2Create(XBOX_IXAudio2** ppXAudio2, u32 Flags, XAUDIO2_PROCESSOR XAudio2Processor) {
    void* xaudio2 = nullptr;
    u32 hr = ::XAudio2Create(&xaudio2, Flags, static_cast<u32>(XAudio2Processor));
    if (hr != 0) return hr;

    std::lock_guard<std::mutex> lock(xaudio2_mutex);
    *ppXAudio2 = new XBOX_IXAudio2(xaudio2);
    xaudio2_instances[xaudio2] = std::unique_ptr<XBOX_IXAudio2>(*ppXAudio2);
    return 0;
}

void XAudio2Release(XBOX_IXAudio2* pXAudio2) {
    if (!pXAudio2) return;
    std::lock_guard<std::mutex> lock(xaudio2_mutex);
    xaudio2_instances.erase(pXAudio2->GetXAudio2());
    delete pXAudio2;
}

} // namespace xbox360 