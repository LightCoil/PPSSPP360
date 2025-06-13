#ifndef DIRECTX_PROFILE_HPP
#define DIRECTX_PROFILE_HPP

#include "directx_defs.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <future>
#include <chrono>
#include <functional>
#include <algorithm>

namespace xbox360 {

class XBOX_IDirect3DProfile9 {
private:
    u32 m_totalProfiles;
    u32 m_activeProfiles;
    u32 m_totalMemory;
    f32 m_averageProfileTime;
    f32 m_maxProfileTime;
    f32 m_minProfileTime;
    std::mutex m_mutex;
    std::vector<std::future<void>> m_futures;
    std::unordered_map<std::string, std::shared_ptr<void>> m_profile_cache;
    size_t m_max_cache_size;

public:
    // Конструктор и деструктор
    XBOX_IDirect3DDevice9(IDirect3DDevice9* device);
    ~XBOX_IDirect3DDevice9();

    // Профилирование
    void StartProfiling();
    void StopProfiling();
    void ResetProfiles();
    std::string GetProfileReport();
    void SaveProfileReport(const std::string& filename);

    // Оптимизация
    void AddOptimizationRule(
        std::function<bool()> condition,
        std::function<void()> action,
        std::chrono::seconds interval = std::chrono::seconds(5));
    void StartOptimizing();
    void StopOptimizing();
    void UpdateOptimizer();
};

} // namespace xbox360

#endif // DIRECTX_PROFILE_HPP 