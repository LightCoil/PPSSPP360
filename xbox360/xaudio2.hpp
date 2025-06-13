#ifndef XAUDIO2_HPP
#define XAUDIO2_HPP

#include <vector>
#include <memory>
#include <mutex>
#include <future>
#include <functional>
#include <unordered_map>
#include <cstdint>
#include <windows.h>
#include <mmsystem.h>

namespace xbox360 {

// Базовые типы
using u32 = uint32_t;
using f32 = float;

// Заглушки для структур XAudio2
struct XAUDIO2_EFFECT_CHAIN {};
struct XAUDIO2_VOICE_SENDS {};
using WAVEFORMATEX = ::WAVEFORMATEX;

// Структуры для работы с буферами
struct XAUDIO2_BUFFER {
    u32     Flags;              // Флаги буфера
    u32     AudioBytes;         // Размер аудио данных в байтах
    const uint8_t* pAudioData;  // Указатель на аудио данные
    u32     PlayBegin;         // Начальная позиция воспроизведения
    u32     PlayLength;        // Длина воспроизведения
    u32     LoopBegin;         // Начальная позиция цикла
    u32     LoopLength;        // Длина цикла
    u32     LoopCount;         // Количество повторений
    void*   pContext;          // Пользовательский контекст

    XAUDIO2_BUFFER() : 
        Flags(0), 
        AudioBytes(0), 
        pAudioData(nullptr),
        PlayBegin(0),
        PlayLength(0),
        LoopBegin(0),
        LoopLength(0),
        LoopCount(0),
        pContext(nullptr) {}
};

struct XAUDIO2_BUFFER_WMA {
    const u32* pDecodedPacketCumulativeBytes;
    u32        PacketCount;

    XAUDIO2_BUFFER_WMA() :
        pDecodedPacketCumulativeBytes(nullptr),
        PacketCount(0) {}
};

// Константы XAudio2
#define XAUDIO2_COMMIT_NOW 0
#define XAUDIO2_DEFAULT_FREQ_RATIO 2.0f

// Предварительные объявления классов
class XBOX_IXAudio2SourceVoice;
class XBOX_IXAudio2MasteringVoice;

using AudioVoice = XBOX_IXAudio2SourceVoice;
using AudioMasteringVoice = XBOX_IXAudio2MasteringVoice;
using XBOX_XAUDIO2_BUFFER = XAUDIO2_BUFFER;

/**
 * @brief Класс для работы с XAudio2 на Xbox 360
 */
class XBOX_IXAudio2 {
private:
    void* m_xaudio2; // Внутренний указатель на XAudio2
    std::vector<std::shared_ptr<XBOX_IXAudio2SourceVoice>> m_source_voices;
    std::mutex m_mutex;

public:
    /**
     * @brief Конструктор
     * @param xaudio2 Указатель на внутренний XAudio2
     */
    XBOX_IXAudio2(void* xaudio2);
    ~XBOX_IXAudio2();

    /**
     * @brief Создает мастер-голос
     */
    u32 CreateMasteringVoice(
        XBOX_IXAudio2MasteringVoice** ppMasteringVoice,
        u32 InputChannels = 2,
        u32 InputSampleRate = 44100,
        u32 Flags = 0,
        u32 DeviceIndex = 0,
        const XAUDIO2_EFFECT_CHAIN* pEffectChain = nullptr
    );

    /**
     * @brief Создает источник звука
     */
    u32 CreateSourceVoice(
        XBOX_IXAudio2SourceVoice** ppSourceVoice,
        const WAVEFORMATEX* pSourceFormat,
        u32 Flags = 0,
        f32 MaxFrequencyRatio = 2.0f,
        void* pCallback = nullptr,
        const XAUDIO2_VOICE_SENDS* pSendList = nullptr,
        const XAUDIO2_EFFECT_CHAIN* pEffectChain = nullptr
    );

    /**
     * @brief Получает внутренний XAudio2
     * @return Указатель на внутренний XAudio2
     */
    void* GetXAudio2() const { return m_xaudio2; }

    /**
     * @brief Очищает все источники звука
     */
    void ClearSourceVoices();

    /**
     * @brief Получает количество источников звука
     * @return Количество источников звука
     */
    u32 GetSourceVoiceCount() const;
};

/**
 * @brief Класс для работы с мастер-голосом на Xbox 360
 */
class XBOX_IXAudio2MasteringVoice {
private:
    void* m_voice; // Внутренний указатель на голос
    f32 m_volume; // Громкость

public:
    /**
     * @brief Конструктор
     * @param voice Указатель на внутренний голос
     */
    XBOX_IXAudio2MasteringVoice(void* voice);
    ~XBOX_IXAudio2MasteringVoice();

    /**
     * @brief Получает громкость
     * @param pVolume Указатель на громкость
     * @return Код результата
     */
    u32 GetVolume(f32* pVolume);

    /**
     * @brief Устанавливает громкость
     * @param Volume Громкость
     * @param OperationSet Набор операций
     * @return Код результата
     */
    u32 SetVolume(f32 Volume, u32 OperationSet = XAUDIO2_COMMIT_NOW);

    /**
     * @brief Получает внутренний голос
     * @return Указатель на внутренний голос
     */
    void* GetVoice() const { return m_voice; }
};

/**
 * @brief Класс для работы с источником звука на Xbox 360
 */
class XBOX_IXAudio2SourceVoice {
private:
    void* m_voice; // Внутренний указатель на голос
    f32 m_volume; // Громкость
    bool m_is_playing; // Флаг воспроизведения
    std::vector<XAUDIO2_BUFFER> m_buffers; // Буферы

public:
    /**
     * @brief Конструктор
     * @param voice Указатель на внутренний голос
     */
    XBOX_IXAudio2SourceVoice(void* voice);
    ~XBOX_IXAudio2SourceVoice();

    /**
     * @brief Начинает воспроизведение
     * @param Flags Флаги
     * @param OperationSet Набор операций
     * @return Код результата
     */
    u32 Start(u32 Flags = 0, u32 OperationSet = XAUDIO2_COMMIT_NOW);

    /**
     * @brief Останавливает воспроизведение
     * @param Flags Флаги
     * @param OperationSet Набор операций
     * @return Код результата
     */
    u32 Stop(u32 Flags = 0, u32 OperationSet = XAUDIO2_COMMIT_NOW);

    /**
     * @brief Отправляет буфер источника
     * @param pBuffer Буфер
     * @param pBufferWMA Буфер WMA
     * @return Код результата
     */
    u32 SubmitSourceBuffer(const XAUDIO2_BUFFER* pBuffer,
        const XAUDIO2_BUFFER_WMA* pBufferWMA = nullptr);

    /**
     * @brief Очищает буферы источника
     * @return Код результата
     */
    u32 FlushSourceBuffers();

    /**
     * @brief Получает громкость
     * @param pVolume Указатель на громкость
     * @return Код результата
     */
    u32 GetVolume(f32* pVolume);

    /**
     * @brief Устанавливает громкость
     * @param Volume Громкость
     * @param OperationSet Набор операций
     * @return Код результата
     */
    u32 SetVolume(f32 Volume, u32 OperationSet = XAUDIO2_COMMIT_NOW);

    /**
     * @brief Получает внутренний голос
     * @return Указатель на внутренний голос
     */
    void* GetVoice() const { return m_voice; }

    /**
     * @brief Проверяет, воспроизводится ли звук
     * @return true, если воспроизводится
     */
    bool IsPlaying() const { return m_is_playing; }

    /**
     * @brief Получает количество буферов
     * @return Количество буферов
     */
    u32 GetBufferCount() const { return static_cast<u32>(m_buffers.size()); }
};

/**
 * @brief Класс для оптимизации звука
 */
class AudioOptimizer {
public:
    struct AudioStats {
        u32 totalVoices;
        u32 activeVoices;
        u32 totalBuffers;
        u32 totalMemory;
        f32 averageLoadTime;
        f32 maxLoadTime;
        f32 minLoadTime;
    };

    /**
     * @brief Конструктор
     * @param xaudio2 XAudio2
     */
    AudioOptimizer(XBOX_IXAudio2* xaudio2);
    ~AudioOptimizer();

    /**
     * @brief Создает источник звука с оптимизацией
     * @param format Формат звука
     * @param voice Указатель на указатель голоса
     * @return Код результата
     */
    u32 CreateSourceVoice(
        const WAVEFORMATEX* format,
        XBOX_IXAudio2SourceVoice** voice);

    /**
     * @brief Предварительно загружает звуки
     * @param formats Вектор форматов звука
     */
    void PreloadSounds(
        const std::vector<WAVEFORMATEX>& formats);

    /**
     * @brief Ожидает завершения загрузки
     */
    void WaitForLoading();

    /**
     * @brief Асинхронно создает источник звука
     * @param format Формат звука
     * @param callback Функция обратного вызова
     * @return Future с результатом
     */
    std::future<u32> CreateSourceVoiceAsync(
        const WAVEFORMATEX* format,
        std::function<void(XBOX_IXAudio2SourceVoice*)> callback);

    /**
     * @brief Устанавливает размер кэша звуков
     * @param max_size Максимальный размер кэша
     */
    void SetCacheSize(size_t max_size);

    /**
     * @brief Получает размер кэша звуков
     * @return Размер кэша
     */
    size_t GetCacheSize() const;

    /**
     * @brief Очищает кэш звуков
     */
    void ClearCache();

    /**
     * @brief Получает статистику звука
     * @return Статистика звука
     */
    AudioStats GetStats() const;

private:
    XBOX_IXAudio2* m_xaudio2;
    std::mutex m_mutex;
    std::vector<std::future<void>> m_futures;
    AudioStats m_stats;
    std::unordered_map<std::string, std::shared_ptr<XBOX_IXAudio2SourceVoice>> m_voice_cache;
    size_t m_max_cache_size;
};

u32 XAudio2Create(XBOX_IXAudio2** ppXAudio2, u32 Flags, u32 XAudio2Processor);
void XAudio2Release(XBOX_IXAudio2* pXAudio2);

inline u32 CreateXAudioSourceVoice(XBOX_IXAudio2* xaudio2, AudioVoice** ppSourceVoice, const WAVEFORMATEX* pSourceFormat, u32 Flags = 0, f32 MaxFrequencyRatio = 2.0f, void* pCallback = nullptr, const XAUDIO2_VOICE_SENDS* pSendList = nullptr, const XAUDIO2_EFFECT_CHAIN* pEffectChain = nullptr) {
    return xaudio2->CreateSourceVoice(reinterpret_cast<XBOX_IXAudio2SourceVoice**>(ppSourceVoice), pSourceFormat, Flags, MaxFrequencyRatio, pCallback, pSendList, pEffectChain);
}

inline void DestroyXAudioSourceVoice(AudioVoice* pSourceVoice) {
    delete pSourceVoice;
}

inline u32 CreateXAudioMasteringVoice(XBOX_IXAudio2* xaudio2, AudioMasteringVoice** ppMasteringVoice, u32 InputChannels = 2, u32 InputSampleRate = 44100, u32 Flags = 0, u32 DeviceIndex = 0, const XAUDIO2_EFFECT_CHAIN* pEffectChain = nullptr) {
    return xaudio2->CreateMasteringVoice(reinterpret_cast<XBOX_IXAudio2MasteringVoice**>(ppMasteringVoice), InputChannels, InputSampleRate, Flags, DeviceIndex, pEffectChain);
}

inline void DestroyXAudioMasteringVoice(AudioMasteringVoice* pMasteringVoice) {
    delete pMasteringVoice;
}

inline u32 SubmitXAudioSourceBuffer(AudioVoice* pSourceVoice, const XBOX_XAUDIO2_BUFFER* pBuffer) {
    return pSourceVoice->SubmitSourceBuffer(reinterpret_cast<const XAUDIO2_BUFFER*>(pBuffer));
}

inline u32 SetXAudioVolume(AudioVoice* pSourceVoice, float volume) {
    return pSourceVoice->SetVolume(volume);
}

inline u32 StartXAudioSourceVoice(AudioVoice* pSourceVoice) {
    return pSourceVoice->Start();
}

inline void XAudioRelease(XBOX_IXAudio2* xaudio2) {
    XAudio2Release(xaudio2);
}

} // namespace xbox360

#endif // XAUDIO2_HPP 