#include <fetchtop/parsers.hpp>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

namespace fetchtop {
    MemoryStatus parse_memory() {
        MemoryStatus mem;
        // Открываем файл. RAII закроет его сам при выходе из функции!
        std::ifstream file("/proc/meminfo"); 

        if (!file.is_open()) {
            std::cerr << "Error: Cannot open /proc/meminfo\n";
            return mem; // Возвращаем нули, чтобы программа не упала
        }

        std::string line;
        while (std::getline(file, line)) {
            // Если нашли нужную строку, читаем из нее значения
            if (line.starts_with("MemTotal:")) {
                std::istringstream iss(line);
                std::string key;
                iss >> key >> mem.total_kb; // iss разбивает строку по пробелам
            } 
            else if (line.starts_with("MemAvailable:")) {
                std::istringstream iss(line);
                std::string key;
                iss >> key >> mem.available_kb;
            }
        }

        return mem;
    }
}