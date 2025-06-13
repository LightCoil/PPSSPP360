#pragma once

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

// Базовые типы
struct PSPState {
    uint32_t pc;
    uint32_t sp;
    uint32_t ra;
    uint32_t v0;
    uint32_t v1;
    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t a3;
    uint32_t t0;
    uint32_t t1;
    uint32_t t2;
    uint32_t t3;
    uint32_t t4;
    uint32_t t5;
    uint32_t t6;
    uint32_t t7;
    uint32_t s0;
    uint32_t s1;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t t8;
    uint32_t t9;
    uint32_t k0;
    uint32_t k1;
    uint32_t gp;
    uint32_t fp;
    uint32_t hi;
    uint32_t lo;
    uint32_t fcr0;
    uint32_t fcr31;
    float f0;
    float f1;
    float f2;
    float f3;
    float f4;
    float f5;
    float f6;
    float f7;
    float f8;
    float f9;
    float f10;
    float f11;
    float f12;
    float f13;
    float f14;
    float f15;
    float f16;
    float f17;
    float f18;
    float f19;
    float f20;
    float f21;
    float f22;
    float f23;
    float f24;
    float f25;
    float f26;
    float f27;
    float f28;
    float f29;
    float f30;
    float f31;
};

// Структуры для статистики
struct PSPStats {
    size_t instruction_count;
    size_t memory_access;
    size_t cache_hits;
    size_t cache_misses;
    size_t branch_count;
    size_t exception_count;
    std::chrono::system_clock::time_point last_reset;
};

// Константы
constexpr size_t MEMORY_SIZE = 32 * 1024 * 1024;
constexpr size_t CACHE_SIZE = 16 * 1024;
constexpr size_t CACHE_LINE_SIZE = 64;
constexpr size_t CACHE_WAYS = 4;
constexpr size_t CACHE_SETS = CACHE_SIZE / (CACHE_LINE_SIZE * CACHE_WAYS);

// Флаги
constexpr uint32_t CPU_FLAGS = 0;
constexpr uint32_t FPU_FLAGS = 0;
constexpr uint32_t VFPU_FLAGS = 0;
constexpr uint32_t CACHE_FLAGS = 0;

// Пороговые значения
constexpr uint32_t BRANCH_DELAY_SLOT = 1;
constexpr uint32_t EXCEPTION_VECTOR = 0x80000000;
constexpr uint32_t KERNEL_MODE = 0x80000000;

} // namespace xbox360 