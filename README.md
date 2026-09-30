# Embedded Linux Device Health Monitor

A lightweight, real-time Linux telemetry daemon developed in C++17 for embedded devices. It reads system statistics directly from Linux kernel pseudo-filesystems (`/proc/stat`, `/proc/meminfo`, `/proc/uptime`), calculates multi-core CPU utilization, available memory usage, and total system uptime, and serializes metrics into structured JSON payloads.

## Features
- **Real-Time CPU Tracking**: Computes active CPU percentage across sampling intervals by parsing CPU jiffies from `/proc/stat`.
- **Memory Consumption Monitoring**: Parses `/proc/meminfo` to derive active memory footprint relative to total available RAM.
- **Uptime Telemetry**: Extracts device runtime statistics via `/proc/uptime`.
- **JSON Payload Formatting**: Integrates `nlohmann/json` for standardized telemetry logging and ease of integration with cloud dashboards.
- **Systemd Service Daemon**: Fully configured to run continuously as a background service managed by `systemd`.

## Repository Structure
```text
device-health-monitor/
├── CMakeLists.txt         # Build system configuration
├── main.cpp               # C++ health monitor core logic
├── device_health.service  # Systemd daemon configuration
└── README.md              # Project documentation

# Prerequisites
​Operating System: Linux / WSL2 (Ubuntu 20.04/22.04 LTS)
​Compiler: g++ with C++17 support
​Build Utilities: cmake (>= 3.10), make
​Libraries: nlohmann-json3-dev
​Install dependencies on Ubuntu/Debian:

sudo apt update && sudo apt install -y build-essential cmake nlohmann-json3-dev
Build and Run
1: clone the repository:

git clone [https://github.com/sraunak839-sketch/device-health-monitor.git](https://github.com/sraunak839-sketch/device-health-monitor.git)
cd device-health-monitor

2: Build using CMake:
mkdir -p build && cd build
cmake ..
make

3:Execute the binary:
./health_monitor

Output Format:

{
  "timestamp": 1790781081,
  "status": "OK",
  "metrics": {
    "cpu_usage_percent": 2.98,
    "memory_usage_percent": 21.95,
    "uptime_seconds": 2182
  }
}

Systemd Daemon Setup:

To register and run the application as an automated background daemon:
sudo cp device_health.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable --now device_health.service

Inspect background log stream:
sudo journalctl -u device_health.service -f 
