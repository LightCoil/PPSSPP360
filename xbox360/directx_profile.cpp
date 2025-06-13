#include "directx_profile.hpp"
#include "directx_defs.hpp"
#include <memory>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <chrono>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

namespace xbox360 {

// Профилировщик производительности
struct PerformanceProfiler {
    struct ProfileData {
        std::chrono::nanoseconds total_time;
        std::chrono::nanoseconds min_time;
        std::chrono::nanoseconds max_time;
        size_t call_count;
        std::chrono::system_clock::time_point last_reset;
    };

    std::unordered_map<std::string, ProfileData> profiles;
    std::mutex profile_mutex;
    std::chrono::system_clock::time_point start_time;
    bool is_profiling;

    PerformanceProfiler()
        : is_profiling(false) {
        start_time = std::chrono::system_clock::now();
    }

    void BeginProfile(const std::string& name) {
        if (!is_profiling) return;

        std::lock_guard<std::mutex> lock(profile_mutex);
        auto& profile = profiles[name];
        profile.last_reset = std::chrono::system_clock::now();
    }

    void EndProfile(const std::string& name) {
        if (!is_profiling) return;

        std::lock_guard<std::mutex> lock(profile_mutex);
        auto now = std::chrono::system_clock::now();
        auto& profile = profiles[name];
        
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
            now - profile.last_reset);
        
        profile.total_time += duration;
        profile.min_time = std::min(profile.min_time, duration);
        profile.max_time = std::max(profile.max_time, duration);
        profile.call_count++;
    }

    void StartProfiling() {
        std::lock_guard<std::mutex> lock(profile_mutex);
        is_profiling = true;
        start_time = std::chrono::system_clock::now();
    }

    void StopProfiling() {
        std::lock_guard<std::mutex> lock(profile_mutex);
        is_profiling = false;
    }

    void ResetProfiles() {
        std::lock_guard<std::mutex> lock(profile_mutex);
        profiles.clear();
    }

    std::string GetProfileReport() {
        std::lock_guard<std::mutex> lock(profile_mutex);
        std::stringstream ss;
        
        ss << "Performance Profile Report\n";
        ss << "========================\n\n";
        
        for (const auto& [name, data] : profiles) {
            ss << name << ":\n";
            ss << "  Total Time: " << data.total_time.count() / 1000000.0 << " ms\n";
            ss << "  Min Time: " << data.min_time.count() / 1000000.0 << " ms\n";
            ss << "  Max Time: " << data.max_time.count() / 1000000.0 << " ms\n";
            ss << "  Call Count: " << data.call_count << "\n";
            ss << "  Average Time: " << 
                (data.total_time.count() / (double)data.call_count) / 1000000.0 << " ms\n\n";
        }
        
        return ss.str();
    }

    void SaveProfileReport(const std::string& filename) {
        std::ofstream file(filename);
        file << GetProfileReport();
    }
};

// Оптимизатор производительности
struct PerformanceOptimizer {
    struct OptimizationRule {
        std::function<bool()> condition;
        std::function<void()> action;
        std::chrono::system_clock::time_point last_check;
        std::chrono::seconds check_interval;
    };

    std::vector<OptimizationRule> rules;
    std::mutex optimizer_mutex;
    bool is_optimizing;

    PerformanceOptimizer()
        : is_optimizing(false) {}

    void AddRule(
        std::function<bool()> condition,
        std::function<void()> action,
        std::chrono::seconds interval = std::chrono::seconds(5)) {
        
        std::lock_guard<std::mutex> lock(optimizer_mutex);
        rules.push_back({
            std::move(condition),
            std::move(action),
            std::chrono::system_clock::now(),
            interval
        });
    }

    void StartOptimizing() {
        std::lock_guard<std::mutex> lock(optimizer_mutex);
        is_optimizing = true;
    }

    void StopOptimizing() {
        std::lock_guard<std::mutex> lock(optimizer_mutex);
        is_optimizing = false;
    }

    void Update() {
        if (!is_optimizing) return;

        std::lock_guard<std::mutex> lock(optimizer_mutex);
        auto now = std::chrono::system_clock::now();
        
        for (auto& rule : rules) {
            if (now - rule.last_check >= rule.check_interval) {
                if (rule.condition()) {
                    rule.action();
                }
                rule.last_check = now;
            }
        }
    }
};

static PerformanceProfiler g_profiler;
static PerformanceOptimizer g_optimizer;

// Начало профилирования
void XBOX_IDirect3DDevice9::StartProfiling() {
    g_profiler.StartProfiling();
}

// Остановка профилирования
void XBOX_IDirect3DDevice9::StopProfiling() {
    g_profiler.StopProfiling();
}

// Сброс профилей
void XBOX_IDirect3DDevice9::ResetProfiles() {
    g_profiler.ResetProfiles();
}

// Получение отчета о производительности
std::string XBOX_IDirect3DDevice9::GetProfileReport() {
    return g_profiler.GetProfileReport();
}

// Сохранение отчета о производительности
void XBOX_IDirect3DDevice9::SaveProfileReport(const std::string& filename) {
    g_profiler.SaveProfileReport(filename);
}

// Добавление правила оптимизации
void XBOX_IDirect3DDevice9::AddOptimizationRule(
    std::function<bool()> condition,
    std::function<void()> action,
    std::chrono::seconds interval) {
    
    g_optimizer.AddRule(std::move(condition), std::move(action), interval);
}

// Запуск оптимизации
void XBOX_IDirect3DDevice9::StartOptimizing() {
    g_optimizer.StartOptimizing();
}

// Остановка оптимизации
void XBOX_IDirect3DDevice9::StopOptimizing() {
    g_optimizer.StopOptimizing();
}

// Обновление оптимизатора
void XBOX_IDirect3DDevice9::UpdateOptimizer() {
    g_optimizer.Update();
}

} // namespace xbox360 