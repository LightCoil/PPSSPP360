#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace ppsspp {
namespace core {

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

class Logger {
public:
    static Logger& Instance() {
        static Logger instance;
        return instance;
    }

    void Init(const std::string& filename = "psp360.log");
    void SetLogLevel(LogLevel level) { currentLevel_ = level; }

    template<typename... Args>
    void Debug(const std::string& format, Args&&... args) {
        Log(LogLevel::DEBUG, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void Info(const std::string& format, Args&&... args) {
        Log(LogLevel::INFO, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void Warning(const std::string& format, Args&&... args) {
        Log(LogLevel::WARNING, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void Error(const std::string& format, Args&&... args) {
        Log(LogLevel::ERROR, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void Fatal(const std::string& format, Args&&... args) {
        Log(LogLevel::FATAL, format, std::forward<Args>(args)...);
    }

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string GetTimestamp() const;
    std::string GetLevelString(LogLevel level) const;
    
    template<typename... Args>
    void Log(LogLevel level, const std::string& format, Args&&... args) {
        if (level < currentLevel_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        if (!logFile_.is_open()) return;

        logFile_ << GetTimestamp() << " [" << GetLevelString(level) << "] ";
        // TODO: Implement format string processing
        logFile_ << format << std::endl;
        logFile_.flush();
    }

    std::ofstream logFile_;
    std::mutex mutex_;
    LogLevel currentLevel_ = LogLevel::INFO;
};

// Глобальные функции для удобства использования
inline void LogDebug(const std::string& message) {
    Logger::Instance().Debug(message);
}

inline void LogInfo(const std::string& message) {
    Logger::Instance().Info(message);
}

inline void LogWarning(const std::string& message) {
    Logger::Instance().Warning(message);
}

inline void LogError(const std::string& message) {
    Logger::Instance().Error(message);
}

inline void LogFatal(const std::string& message) {
    Logger::Instance().Fatal(message);
}

} // namespace core
} // namespace ppsspp 