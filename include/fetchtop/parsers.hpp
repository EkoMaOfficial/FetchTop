#pragma once
#include "system.hpp"
#include <vector>

namespace fetchtop {
    MemoryStatus parse_memory();
    
    CpuData parse_cpu();

    std::vector<GpuData> parse_gpu();
    
    std::vector<ProcessInfo> parse_processes(); 
}