#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <fetchtop/parsers.hpp>

// Состояния приложения
enum class AppState {
    Menu,
    DefaultView,
    AdvancedView,
    Exit
};

// Функция отрисовки главного меню
void render_menu() {
    std::cout << "\033[2J\033[H";
    std::cout << R"(
  _____ ____ _____ ____ _   _ _____ ___  ____  
 |  ___| ____|_   _/ ___| | | |_   _/ _ \|  _ \ 
 | |_  |  _|   | || |   | |_| | | || | | | |_) |
 |  _| | |___  | || |___|  _  | | || |_| |  __/ 
 |_|   |_____| |_| \____|_| |_| |_| \___/|_|    
    )" << "\n\n";

    std::cout << "1. Default View (General Info)\n";
    std::cout << "2. Advanced View (Per-Core & Extended Processes)\n";
    std::cout << "3. Exit\n\n";
    std::cout << "Select option [1-3]: ";
}

// Рендеринг основного мониторинга (Default)
void run_default_view() {
    std::cout << "\033[2J";
    
    // Бесконечный цикл дашборда (выход по Ctrl+C)
    while (true) {
        fetchtop::CpuData cpu_old = fetchtop::parse_cpu();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        fetchtop::CpuData cpu_new = fetchtop::parse_cpu();

        fetchtop::MemoryStatus mem = fetchtop::parse_memory();
        std::vector<fetchtop::ProcessInfo> processes = fetchtop::parse_processes();

        std::cout << "\033[H";
        std::cout << "=== FetchTop: Default View ===\033[K\n\n";

        std::cout << "RAM: " << mem.available_kb / 1024 << " MB free @ " 
                  << mem.total_kb / 1024 << " MB\033[K\n";

        std::uint64_t total_delta = cpu_new.total_time - cpu_old.total_time;
        std::uint64_t idle_delta = cpu_new.idle_time - cpu_old.idle_time;
        double cpu_usage = (total_delta > 0) ? 100.0 * (total_delta - idle_delta) / total_delta : 0.0;

        std::cout << std::fixed << std::setprecision(1);
        std::cout << "CPU: " << cpu_usage << " %\033[K\n\n";

        std::cout << "--- TOP 5 PROCESSES ---\033[K\n";
        std::cout << "  PID\t\tRAM (MB)\tNAME\033[K\n";

        size_t count = std::min<size_t>(5, processes.size());
        for (size_t i = 0; i < count; ++i) {
            std::cout << "  " << processes[i].pid << "\t\t" 
                      << processes[i].ram_kb / 1024 << " MB\t\t" 
                      << processes[i].name << "\033[K\n";
        }
    }
}

// Рендеринг расширенного мониторинга (Advanced)
void run_advanced_view() {
    std::cout << "\033[2J";
    
    while (true) {
        // 1. Сбор данных
        fetchtop::CpuData cpu_old = fetchtop::parse_cpu();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        fetchtop::CpuData cpu_new = fetchtop::parse_cpu();

        fetchtop::MemoryStatus mem = fetchtop::parse_memory();
        std::vector<fetchtop::ProcessInfo> processes = fetchtop::parse_processes();
        
        // ЗАПРАШИВАЕМ ДАННЫЕ GPU
        std::vector<fetchtop::GpuData> gpus = fetchtop::parse_gpu();

        // 2. Отрисовка интерфейса
        std::cout << "\033[H";
        std::cout << "=== FetchTop: Advanced View ===\033[K\n\n";

        std::cout << "RAM Usage: " << mem.available_kb / 1024 << " MB free out of " 
                  << mem.total_kb / 1024 << " MB total\033[K\n";

        // Математика CPU (только ОДИН раз!)
        std::uint64_t total_delta = cpu_new.total_time - cpu_old.total_time;
        std::uint64_t idle_delta = cpu_new.idle_time - cpu_old.idle_time;
        double cpu_usage = (total_delta > 0) ? 100.0 * (total_delta - idle_delta) / total_delta : 0.0;

        std::cout << std::fixed << std::setprecision(1);
        std::cout << "Total CPU Load: " << cpu_usage << " % [" 
                  << cpu_new.current_ghz << " GHz @ " << cpu_new.max_ghz << " GHz]\033[K\n";

        // Отрисовка GPU
        for (const auto& gpu : gpus) {
            std::cout << "GPU (" << gpu.name << "): " << gpu.usage_percent << " %";
            if (gpu.current_mhz > 0) {
                std::cout << " @ " << gpu.current_mhz << " MHz";
            }
            std::cout << "\033[K\n";
        }
        std::cout << "\n";

        std::cout << "--- TOP 25 PROCESSES BY RAM ---\033[K\n";
        std::cout << "  PID\t\tRAM (MB)\tNAME\033[K\n";

        // Вывод процессов
        size_t count = std::min<size_t>(25, processes.size());
        for (size_t i = 0; i < count; ++i) {
            std::cout << "  " << processes[i].pid << "\t\t" 
                      << processes[i].ram_kb / 1024 << " MB\t\t" 
                      << processes[i].name << "\033[K\n";
        }
    }
}

int main() {
    AppState state = AppState::Menu;

    while (state != AppState::Exit) {
        switch (state) {
            case AppState::Menu: {
                render_menu();
                int choice = 0;
                if (std::cin >> choice) {
                    if (choice == 1) state = AppState::DefaultView;
                    else if (choice == 2) state = AppState::AdvancedView;
                    else if (choice == 3) state = AppState::Exit;
                } else {
                    // Очистка при нечисловом вводе
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                }
                break;
            }
            case AppState::DefaultView:
                run_default_view();
                break;

            case AppState::AdvancedView:
                run_advanced_view();
                break;

            case AppState::Exit:
                break;
        }
    }

    std::cout << "\033[2J\033[HExiting FetchTop. Goodbye!\n";
    return 0;
}