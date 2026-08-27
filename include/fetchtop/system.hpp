#pragma once
#include <cstdint>
#include <string>

namespace fetchtop {
    struct MemoryStatus {
        std::uint64_t total_kb = 0;
        std::uint64_t available_kb = 0;
    };

    struct CpuData {
        std::uint64_t idle_time = 0;
        std::uint64_t total_time = 0;

        std::uint64_t current_ghz = 0.0;
        std::uint64_t max_ghz = 0.0;
    };

    struct GpuData {
        std::string name;
        int usage_percent = 0;
        double current_mhz = 0.0;
    };

    struct ProcessInfo {
        int pid = 0;
        std::string name;
        std::uint64_t ram_kb = 0;
    };
}