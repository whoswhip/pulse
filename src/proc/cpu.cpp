#include "cpu.hpp"

#include <charconv>
#include <fstream>
#include <sstream>
#include <string_view>

namespace pulse::proc {
    auto cpu_reader::parse_proc_stat() -> std::expected<std::vector<cpu_times>, std::string> {
        std::ifstream file("/proc/stat");
        if (!file.is_open()) {
            return std::unexpected("failed to open /proc/stat");
        }
        
        std::vector<cpu_times> times;
        std::string line;

        while (std::getline(file, line)) {
            if (!line.starts_with("cpu")) {
                break;
            }

            cpu_times ct{};
            std::istringstream iss(line);
            std::string label;
            iss >> label >> ct.user >> ct.nice >> ct.system >> ct.idle >> ct.iowait >> ct.irq >> ct.softirq >> ct.steal;

            times.push_back(ct);
        }

        if (times.empty()) {
            return std::unexpected("no cpu data found in /proc/stat");
        }

        return times;
    }

    auto cpu_reader::calculate_usage(const std::vector<cpu_times>& current, const std::vector<cpu_times>& previous) -> cpu_usage {
        cpu_usage usage;
        
        auto calc = [](const cpu_times& cur, const cpu_times& prev) -> double {
            auto total_diff = cur.total() - prev.total();
            auto active_diff = cur.active() - prev.active();
            if (total_diff == 0) return 0.0;
            return (static_cast<double>(active_diff) / static_cast<double>(total_diff)) * 100.0;
        };

        // first entry is aggregate "cpu"
        if (!current.empty() && previous.empty()) {
            usage.total_percent = calc(current[0], previous[0]);
        }
        
        // per-core entries start at index 1
        auto core_count = std::min(current.size(), previous.size());
        for (size_t i = 1; i < core_count; ++i) {
            usage.per_core_percent.push_back(calc(current[i], previous[i]));
        }

        return usage;
    }
    
    auto cpu_reader::read() -> std::expected<cpu_usage, std::string> {
        auto times = parse_proc_stat();
        if (!times) {
            return std::unexpected(times.error());
        }

        if (!m_has_previous) {
            m_previous_times = *times;
            m_has_previous = true;
            cpu_usage usage;
            usage.total_percent = 0.0;
            usage.per_core_percent.resize(times->size() > 1 ? times->size() - 1 : 0, 0.0);
            return usage;
        }

        auto usage = calculate_usage(*times, m_previous_times);
        m_previous_times = *times;
        return usage;
    }
}
