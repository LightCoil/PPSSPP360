#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>

namespace ppsspp {
namespace core {

class Config {
public:
    static Config& GetInstance();

    // Запрещаем копирование
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    // Загрузка/сохранение конфигурации
    bool Load(const std::string& filename);
    bool Save(const std::string& filename) const;

    // Настройки эмулятора
    struct EmulatorSettings {
        bool enableJIT = true;
        bool enableVFPU = true;
        bool enableAudio = true;
        bool enableVideo = true;
        int frameRateLimit = 60;
        int audioSampleRate = 44100;
        int audioBufferSize = 2048;
    } emulator;

    // Настройки отладки
    struct DebugSettings {
        bool enableLogging = true;
        bool enableBreakpoints = false;
        bool enableMemoryWatch = false;
        std::string logLevel = "info";
    } debug;

    // Настройки путей
    struct PathSettings {
        std::string gameDirectory;
        std::string saveDirectory;
        std::string configDirectory;
        std::string logDirectory;
    } paths;

    // Методы для работы с настройками
    template<typename T>
    void SetValue(const std::string& key, const T& value) {
        settings_[key] = value;
    }

    template<typename T>
    std::optional<T> GetValue(const std::string& key) const {
        auto it = settings_.find(key);
        if (it == settings_.end()) {
            return std::nullopt;
        }
        try {
            return std::get<T>(it->second);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }

    // Предустановленные значения
    struct Defaults {
        static constexpr uint32_t INITIAL_PC = 0x08800000;
        static constexpr uint32_t INITIAL_GP = 0x08800000;
        static constexpr bool ENABLE_LOGGING = true;
        static constexpr int LOG_LEVEL = 1; // 0=DEBUG, 1=INFO, 2=WARNING, 3=ERROR, 4=FATAL
        static constexpr int FRAME_LIMIT = 60;
        static constexpr bool ENABLE_SOUND = true;
        static constexpr bool ENABLE_SAVE_STATES = true;
    };

private:
    Config() = default;
    ~Config() = default;

    void LoadDefaults();
    void LoadFromJson(const nlohmann::json& j);
    nlohmann::json SaveToJson() const;

    using SettingValue = std::variant<bool, int, uint32_t, std::string>;
    std::unordered_map<std::string, SettingValue> settings_;
};

// Глобальные функции для удобства использования
inline bool LoadConfig(const std::string& filename) {
    return Config::GetInstance().Load(filename);
}

inline bool SaveConfig(const std::string& filename) {
    return Config::GetInstance().Save(filename);
}

template<typename T>
inline void SetConfigValue(const std::string& key, const T& value) {
    Config::GetInstance().SetValue(key, value);
}

template<typename T>
inline std::optional<T> GetConfigValue(const std::string& key) {
    return Config::GetInstance().GetValue<T>(key);
}

} // namespace core
} // namespace ppsspp 