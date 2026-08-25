#pragma once
#include <cstdint>

namespace fetchtop {    
    struct MemoryStatus {
        std::uint64_t total_kb = 0;
        std::uint64_t available_kb = 0;
    };
}