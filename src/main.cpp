#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <fetchtop/parsers.hpp>

int main() {
    // 1. Очищаем экран ОДИН раз при запуске программы
    std::cout << "\033[2J";

    while (true) {
        fetchtop::CpuData cpu_old = fetchtop::parse_cpu();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        fetchtop::CpuData cpu_new = fetchtop::parse_cpu();

        fetchtop::MemoryStatus mem = fetchtop::parse_memory();
        std::vector<fetchtop::ProcessInfo> processes = fetchtop::parse_processes();

        // 2. Сбрасываем курсор в верхний угол вместо очистки экрана (убираем мерцание!)
        std::cout << "\033[H";

        // \033[K в конце каждой строки затирает хвосты предыдущего вывода
        std::cout << "=== FetchTop System Monitor ===\033[K\n\n";

        std::cout << "RAM: " << mem.available_kb / 1024 << " MB / " 
                  << mem.total_kb / 1024 << " MB\033[K\n";

        std::uint64_t total_delta = cpu_new.total_time - cpu_old.total_time;
        std::uint64_t idle_delta = cpu_new.idle_time - cpu_old.idle_time;
        double cpu_usage = (total_delta > 0) ? 100.0 * (total_delta - idle_delta) / total_delta : 0.0;

        std::cout << std::fixed << std::setprecision(1);
        std::cout << "CPU: " << cpu_usage << " %\033[K\n\n";

        std::cout << "--- TOP 5 PROCESSES BY RAM ---\033[K\n";
        std::cout << "  PID\t\tRAM (MB)\tNAME\033[K\n";

        // Выводим только первые 5 процессов из отсортированного списка
        size_t count = std::min<size_t>(15, processes.size());
        for (size_t i = 0; i < count; ++i) {
            std::cout << "  " << processes[i].pid << "\t\t" 
                      << processes[i].ram_kb / 1024 << " MB\t\t" 
                      << processes[i].name << "\033[K\n";
        }
    }

    return 0;
}