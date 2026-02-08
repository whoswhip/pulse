#pragma once

#include <array>
#include <cstdint>
#include <expected>
#include <string>
#include <sys/types.h>
#include <vector>

namespace pulse::proc {
    
    struct cpu_times {
        uint64_t user{};
        uint64_t nice{};
        uint64_t system{};
        uint64_t idle{};
        uint64_t iowait{};
        uint64_t irq{};
        uint64_t softirq{};
        uint64_t steal{};

        [[nodiscard]] auto total() const -> uint64_t {
            return user + nice + system + idle + iowait + irq + softirq + steal;
        }

        [[nodiscard]] auto active() const -> uint64_t {
            return total() - idle - iowait;
        }
    };

    struct cpu_usage {
        double total_percent{};
        std::vector<double> per_core_percent;
    };

    class cpu_reader {
        public:
            [[nodiscard]] auto read() -> std::expected<cpu_usage, std::string>;

        private:
            [[nodiscard]] auto parse_proc_stat() -> std::expected<std::vector<cpu_times>, std::string>;
            [[nodiscard]] auto calculate_usage(const std::vector<cpu_times>& current, const std::vector<cpu_times>& previous) -> cpu_usage;
        
            std::vector<cpu_times> m_previous_times;
            bool m_has_previous{false};
    };
}
