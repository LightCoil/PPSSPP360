#include "config.h"
#include "logger.h"
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace ppsspp {
namespace core {

Config& Config::GetInstance() {
    static Config instance;
    return instance;
}

void Config::LoadDefaults() {
    // Настройки эмулятора по умолчанию
    emulator.enableJIT = true;
    emulator.enableVFPU = true;
    emulator.enableAudio = true;
    emulator.enableVideo = true;
    emulator.frameRateLimit = 60;
    emulator.audioSampleRate = 44100;
    emulator.audioBufferSize = 2048;

    // Настройки отладки по умолчанию
    debug.enableLogging = true;
    debug.enableBreakpoints = false;
    debug.enableMemoryWatch = false;
    debug.logLevel = "info";

    // Пути по умолчанию
    paths.gameDirectory = "games";
    paths.saveDirectory = "saves";
    paths.configDirectory = "config";
    paths.logDirectory = "logs";
}

bool Config::Load(const std::string& filename) {
    try {
        if (!std::filesystem::exists(filename)) {
            LoadDefaults();
            return Save(filename);
        }

        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        json j;
        file >> j;
        LoadFromJson(j);
        return true;
    }
    catch (const std::exception&) {
        LoadDefaults();
        return false;
    }
}

bool Config::Save(const std::string& filename) const {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        json j = SaveToJson();
        file << j.dump(4);
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

void Config::LoadFromJson(const json& j) {
    // Загружаем настройки эмулятора
    if (j.contains("emulator")) {
        const auto& e = j["emulator"];
        emulator.enableJIT = e.value("enableJIT", true);
        emulator.enableVFPU = e.value("enableVFPU", true);
        emulator.enableAudio = e.value("enableAudio", true);
        emulator.enableVideo = e.value("enableVideo", true);
        emulator.frameRateLimit = e.value("frameRateLimit", 60);
        emulator.audioSampleRate = e.value("audioSampleRate", 44100);
        emulator.audioBufferSize = e.value("audioBufferSize", 2048);
    }

    // Загружаем настройки отладки
    if (j.contains("debug")) {
        const auto& d = j["debug"];
        debug.enableLogging = d.value("enableLogging", true);
        debug.enableBreakpoints = d.value("enableBreakpoints", false);
        debug.enableMemoryWatch = d.value("enableMemoryWatch", false);
        debug.logLevel = d.value("logLevel", "info");
    }

    // Загружаем пути
    if (j.contains("paths")) {
        const auto& p = j["paths"];
        paths.gameDirectory = p.value("gameDirectory", "games");
        paths.saveDirectory = p.value("saveDirectory", "saves");
        paths.configDirectory = p.value("configDirectory", "config");
        paths.logDirectory = p.value("logDirectory", "logs");
    }
}

json Config::SaveToJson() const {
    json j;

    // Сохраняем настройки эмулятора
    j["emulator"] = {
        {"enableJIT", emulator.enableJIT},
        {"enableVFPU", emulator.enableVFPU},
        {"enableAudio", emulator.enableAudio},
        {"enableVideo", emulator.enableVideo},
        {"frameRateLimit", emulator.frameRateLimit},
        {"audioSampleRate", emulator.audioSampleRate},
        {"audioBufferSize", emulator.audioBufferSize}
    };

    // Сохраняем настройки отладки
    j["debug"] = {
        {"enableLogging", debug.enableLogging},
        {"enableBreakpoints", debug.enableBreakpoints},
        {"enableMemoryWatch", debug.enableMemoryWatch},
        {"logLevel", debug.logLevel}
    };

    // Сохраняем пути
    j["paths"] = {
        {"gameDirectory", paths.gameDirectory},
        {"saveDirectory", paths.saveDirectory},
        {"configDirectory", paths.configDirectory},
        {"logDirectory", paths.logDirectory}
    };

    return j;
}

} // namespace core
} // namespace ppsspp 