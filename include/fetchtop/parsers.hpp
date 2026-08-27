#pragma once
#include "system.hpp" // КРИТИЧЕСКИ ВАЖНО: Подключаем наши структуры
#include <vector>     // Для std::vector

namespace fetchtop {
    MemoryStatus parse_memory();
    
    CpuData parse_cpu();
    
    // ТОЛЬКО ОБЪЯВЛЕНИЕ! Никаких фигурных скобок.
    std::vector<ProcessInfo> parse_processes(); 
}