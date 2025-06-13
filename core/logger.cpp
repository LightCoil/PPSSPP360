#include "logger.h"
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <iostream>

namespace ppsspp {
namespace core {

void Logger::Init(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Создаем директорию для логов, если её нет
    std::filesystem::path logPath = std::filesystem::path(filename).parent_path();
    if (!logPath.empty()) {
        std::filesystem::create_directories(logPath);
    }

    logFile_.open(filename, std::ios::app);
    if (!logFile_.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
        return;
    }

    LogInfo("Logger initialized");
}

std::string Logger::GetTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string Logger::GetLevelString(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::FATAL:   return "FATAL";
        default:               return "UNKNOWN";
    }
}

} // namespace core
} // namespace ppsspp 