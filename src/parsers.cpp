#include <fetchtop/parsers.hpp>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <algorithm>

namespace fetchtop {
    namespace fs = std::filesystem;

    // --- ПАРСЕР ПАМЯТИ ---
    MemoryStatus parse_memory() {
        MemoryStatus mem;
        std::ifstream file("/proc/meminfo"); 
        if (!file.is_open()) return mem; 

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
        if (!file.is_open()) return cpu;

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

       // Чтение частоты CPU (Текущая) - с фолбэком!
        std::ifstream cur_freq_file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");
        if (cur_freq_file.is_open()) {
            std::uint64_t freq_khz = 0;
            if (cur_freq_file >> freq_khz) {
                // Явно приводим к double, чтобы спастись от округления до нуля
                cpu.current_ghz = static_cast<double>(freq_khz) / 1000000.0;
            }
        } else {
            // ФОЛБЭК: Если файла нет, читаем из /proc/cpuinfo
            std::ifstream cpuinfo("/proc/cpuinfo");
            std::string line_info;
            while (std::getline(cpuinfo, line_info)) {
                if (line_info.starts_with("cpu MHz")) {
                    std::istringstream iss(line_info);
                    std::string k1, k2, colon;
                    double mhz = 0;
                    if (iss >> k1 >> k2 >> colon >> mhz) {
                        cpu.current_ghz = mhz / 1000.0; // Тут уже мегагерцы, делим на 1000
                        break;
                    }
                }
            }
        }

        // Чтение частоты CPU (Максимальная)
        std::ifstream max_freq_file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq");
        if (max_freq_file.is_open()) {
            std::uint64_t freq_khz = 0;
            if (max_freq_file >> freq_khz) {
                cpu.max_ghz = static_cast<double>(freq_khz) / 1000000.0;
            }
        }

        return cpu;
    } // Конец функции parse_cpu()

    // --- ПАРСЕР ВИДЕОКАРТЫ (GPU) ---
    std::vector<GpuData> parse_gpu() {
        std::vector<GpuData> gpus;
        if (!fs::exists("/sys/class/drm")) return gpus;

       for (const auto& entry : fs::directory_iterator("/sys/class/drm")) {
            std::string folder = entry.path().filename().string();
            
            // НОВОЕ УСЛОВИЕ: берем только "card...", в которых НЕТ дефиса
            if (folder.starts_with("card") && folder.find('-') == std::string::npos) {
                GpuData gpu;
                gpu.name = folder;

                std::ifstream usage_file(entry.path() / "device/gpu_busy_percent");
                if (usage_file.is_open()) {
                    usage_file >> gpu.usage_percent;
                }

                std::ifstream freq_file(entry.path() / "gt_cur_freq_mhz");
                if (freq_file.is_open()) {
                    freq_file >> gpu.current_mhz;
                }

                gpus.push_back(gpu);
            }
        }
        return gpus;
    }

    // --- ПАРСЕР ПРОЦЕССОВ ---
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

        std::sort(processes.begin(), processes.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
            return a.ram_kb > b.ram_kb;
        });

        return processes;
    }

} // Конец namespace fetchtop