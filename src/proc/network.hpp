#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>

namespace pulse::proc {

    struct net_interface {
        std::string name;
        uint64_t rx_bytes{};
        uint64_t tx_bytes{};
        double rx_rate_bytes_sec{};
        double tx_rate_bytes_sec{};
    };

    class network_reader {
        public:
            [[nodiscard]] auto read() -> std::expected<std::vector<net_interface>, std::string>;
        private:
            struct prev_data {
                uint64_t rx_bytes{};
                uint64_t tx_byes{};
            };
            
            std::unordered_map<std::string, prev_data> m_previous;
            std::chrono::steady_clock::time_point m_last_read;
            bool m_has_previous{false};
    };
}
