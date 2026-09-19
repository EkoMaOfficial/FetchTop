# FetchTop

A lightweight, zero-dependency, real-time terminal system monitor for Linux written in modern **C++20**.

```text
                                                                  _nnnn_
                                                                dGGGGMMb
 _____ _____ _____  ____ _   _ _____  ___  ____                @p~qp~~qMb
|  ___| ____|_   _|/ ___| | | |_   _|/ _ \|  _ \               M|@||@) M|
| |_  |  _|   | | | |   | |_| | | | | | | | |_) |              @,----.JM|
|  _| | |___  | | | |___|  _  | | | | |_| |  __/              JS^\__/  qKL
|_|   |_____| |_|  \____|_| |_| |_|  \___/|_|                dZP        qKRb
                                                            dZP          qKKb
     Made by esox <3                                       fZP            SMMb
                                                           HZM            MMMM
  1. Default View (General Info)                           FqM            MMMM
  2. Advanced View (Per-Core & Extended Processes)       __| ".        |\dS"qML
  3. Exit                                                |    `.       | `' \Zq
                                                        _)      \.___.,|     .'
                                                        \____   )MMMMMP|   .'
                                                             `-'       `--'
```

---

## Features

- **Zero External Dependencies**: Built entirely using standard C++20 library features (`<filesystem>`, `<chrono>`, `<thread>`, etc.) and native Linux virtual filesystems.
- **Real-Time CPU Usage**: Computes active vs. idle CPU delta intervals dynamically across polling cycles.
- **CPU Frequency Tracking**: Reads live and maximum CPU scaling frequencies via `/sys/devices/system/cpu/` (with automatic fallback to `/proc/cpuinfo`).
- **Memory Consumption**: Monitors real-time free and total RAM via `/proc/meminfo`.
- **GPU Metrics**: Detects DRM cards via `/sys/class/drm` and reports GPU busy percentage and clock frequency.
- **Process Inspection**: Iterates `/proc/[pid]` to report and rank running processes by resident memory usage (`VmRSS`).
- **Dual View Modes**:
  - **Default View**: Quick summary of CPU load, RAM usage, and top 5 memory-heavy processes.
  - **Advanced View**: Detailed system load with CPU clock frequencies, GPU utilization, and top 25 memory-heavy processes.

---

## Requirements

- **Operating System**: Linux (requires `/proc` and `/sys` filesystems)
- **Compiler**: C++20 compliant compiler (GCC 10+, Clang 12+)
- **Build System**: CMake (version 3.20 or newer)

---

## Build and Run

### 1. Clone the repository
```bash
git clone https://github.com/<username>/FetchTop.git
cd FetchTop
```

### 2. Build with CMake
```bash
mkdir -p build && cd build
cmake ..
cmake --build .
```

### 3. Run
```bash
./fetchtop
```

---

## Project Structure

```text
FetchTop/
├── CMakeLists.txt              # CMake build configuration
├── include/
│   └── fetchtop/
│       ├── dummy.hpp           # Easter egg header
│       ├── parsers.hpp         # System metrics parsing interface
│       └── system.hpp          # Data structures (CpuData, MemoryStatus, GpuData, ProcessInfo)
└── src/
    ├── main.cpp                # Application entry point, CLI rendering & view loops
    └── parsers.cpp             # Linux /proc and /sys file parsers implementation
```

---

## How It Works

| Metric | Source | Description |
| :--- | :--- | :--- |
| **CPU Usage** | `/proc/stat` | Calculated via delta between consecutive sample points: `100 * (total_delta - idle_delta) / total_delta` |
| **CPU Frequency** | `/sys/devices/system/cpu/cpu0/cpufreq/` | Parses `scaling_cur_freq` and `scaling_max_freq` (with fallback to `cpu MHz` in `/proc/cpuinfo`) |
| **Memory** | `/proc/meminfo` | Reads `MemTotal` and `MemAvailable` |
| **GPU Utilization** | `/sys/class/drm/card*/` | Reads `device/gpu_busy_percent` and `gt_cur_freq_mhz` |
| **Process List** | `/proc/[pid]/status` | Parses `Name:` and `VmRSS:`, sorting processes descending by memory footprint |

---

## Author

Created by **esox**.
