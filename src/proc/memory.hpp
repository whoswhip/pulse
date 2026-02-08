#pragma once

#include <cstdint>
#include <expected>
#include <string>

namespace pulse::proc {
    struct memory_info {
        uint64_t total_kb{};
        uint64_t available_kb{};
        uint64_t used_kb{};
        uint64_t buffers_kb{};
        uint64_t cached_kb{};
        uint64_t swap_total_kb{};
        uint64_t swap_used_kb{};

        [[nodiscard]] auto used_percent() const -> double {
            if (total_kb == 0) return 0.0;
            return (static_cast<double>(used_kb) / static_cast<double>(total_kb)) * 100.0;
        }

        [[nodiscard]] auto swap_percent() const -> double {
            if (total_kb == 0) return 0.0;
            return (static_cast<double>(swap_used_kb) / static_cast<double>(swap_total_kb)) * 100.0;
        }
    };

    class memory_reader {
        public:
            [[nodiscard]] auto read() -> std::expected<memory_info, std::string>;
    };
}
