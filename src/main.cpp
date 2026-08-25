#include <iostream>
#include <fetchtop/parsers.hpp>

int main() {    
    std::cout << "---FetchTop---\n";

    fetchtop::MemoryStatus mem = fetchtop::parse_memory();

    std::cout << "Total RAM : " << mem.total_kb / 1024 << " MB\n";
    std::cout << "Available RAM : " << mem.available_kb / 1024 << " MB\n";
    
    return 0;
}