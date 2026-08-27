#include <fetchtop/parsers.hpp>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <algorithm>

namespace fetchtop {

    // --- ПАРСЕР ПАМЯТИ ---
    MemoryStatus parse_memory() {
        MemoryStatus mem;
        std::ifstream file("/proc/meminfo"); 

        if (!file.is_open()) {
            return mem; 
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.starts_with("MemTotal:")) {
                std::istringstream iss(line);
                std::string key;
                iss >> key >> mem.total_kb; 
            } 
            else if (line.starts_with("MemAvailable:")) {
                std::istringstream iss(line);
                std::string key;
                iss >> key >> mem.available_kb;
            }
        }
        return mem;
    }

    // --- ПАРСЕР ПРОЦЕССОРА ---
    CpuData parse_cpu() {
        CpuData cpu;
        std::ifstream file("/proc/stat");

        if (!file.is_open()) {
            return cpu;
        }

        std::string line;
        if (std::getline(file, line) && line.starts_with("cpu ")) {
            std::istringstream iss(line);
            std::string label;
            iss >> label; 

            std::uint64_t user, nice, system, idle, iowait, irq, softirq, steal;
            
            if (iss >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal) {
                cpu.idle_time = idle + iowait;
                cpu.total_time = user + nice + system + idle + iowait + irq + softirq + steal;
            }
        }
        return cpu;
    }

    // --- ПАРСЕР ПРОЦЕССОВ ---
    namespace fs = std::filesystem;

    std::vector<ProcessInfo> parse_processes() {
        std::vector<ProcessInfo> processes;

        for (const auto& entry : fs::directory_iterator("/proc")) {
            if (!entry.is_directory()) continue;

            std::string folder_name = entry.path().filename().string();

            if (std::all_of(folder_name.begin(), folder_name.end(), ::isdigit)) {
                int pid = std::stoi(folder_name);
                
                std::ifstream status_file(entry.path() / "status");
                if (!status_file.is_open()) continue;

                ProcessInfo proc;
                proc.pid = pid;

                std::string line;
                while (std::getline(status_file, line)) {
                    if (line.starts_with("Name:")) {
                        std::istringstream iss(line);
                        std::string key;
                        iss >> key >> proc.name;
                    } 
                    else if (line.starts_with("VmRSS:")) {
                        std::istringstream iss(line);
                        std::string key;
                        iss >> key >> proc.ram_kb;
                    }
                }
                processes.push_back(proc);
            }
        }

        // Сортировка по RAM
        std::sort(processes.begin(), processes.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
            return a.ram_kb > b.ram_kb;
        });

        return processes;
    }

} // namespace fetchtop