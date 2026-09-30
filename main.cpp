#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <vector>
#include <numeric>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Struct to hold raw CPU ticks from /proc/stat
struct CpuData {
    size_t user, nice, system, idle, iowait, irq, softirq, steal;

    size_t get_idle_time() const {
        return idle + iowait;
    }

    size_t get_total_time() const {
        return user + nice + system + idle + iowait + irq + softirq + steal;
    }
};

// Function to read raw CPU times
CpuData get_cpu_data() {
    std::ifstream stat_file("/proc/stat");
    std::string label;
    CpuData cpu{};
    if (stat_file >> label >> cpu.user >> cpu.nice >> cpu.system >> cpu.idle 
                  >> cpu.iowait >> cpu.irq >> cpu.softirq >> cpu.steal) {
        return cpu;
    }
    return {};
}

// Function to calculate active CPU utilization percentage
double calculate_cpu_usage(const CpuData& prev, const CpuData& curr) {
    double prev_idle = static_cast<double>(prev.get_idle_time());
    double curr_idle = static_cast<double>(curr.get_idle_time());

    double prev_total = static_cast<double>(prev.get_total_time());
    double curr_total = static_cast<double>(curr.get_total_time());

    double total_delta = curr_total - prev_total;
    double idle_delta = curr_idle - prev_idle;

    if (total_delta == 0) return 0.0;
    return ((total_delta - idle_delta) / total_delta) * 100.0;
}

// Function to fetch system memory usage
double get_memory_usage() {
    std::ifstream meminfo("/proc/meminfo");
    std::string key;
    long total_mem = 0, free_mem = 0, buffers = 0, cached = 0;

    while (meminfo >> key) {
        if (key == "MemTotal:") meminfo >> total_mem;
        else if (key == "MemFree:") meminfo >> free_mem;
        else if (key == "Buffers:") meminfo >> buffers;
        else if (key == "Cached:") meminfo >> cached;
    }

    if (total_mem == 0) return 0.0;
    long used_mem = total_mem - (free_mem + buffers + cached);
    return (static_cast<double>(used_mem) / total_mem) * 100.0;
}

// Function to fetch system uptime
long get_uptime() {
    std::ifstream uptime_file("/proc/uptime");
    double uptime_seconds = 0;
    if (uptime_file >> uptime_seconds) {
        return static_cast<long>(uptime_seconds);
    }
    return 0;
}

int main() {
    std::cout << "Starting Enhanced Embedded Linux Health Monitor...\n\n";

    CpuData prev_cpu = get_cpu_data();

    for (int i = 0; i < 5; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        CpuData curr_cpu = get_cpu_data();
        double cpu_usage = calculate_cpu_usage(prev_cpu, curr_cpu);
        prev_cpu = curr_cpu;

        json health_report;
        health_report["timestamp"] = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        health_report["status"] = (cpu_usage > 90.0 || get_memory_usage() > 90.0) ? "WARNING" : "OK";
        health_report["metrics"]["cpu_usage_percent"] = cpu_usage;
        health_report["metrics"]["memory_usage_percent"] = get_memory_usage();
        health_report["metrics"]["uptime_seconds"] = get_uptime();

        std::cout << health_report.dump(4) << std::endl;
    }

    return 0;
}