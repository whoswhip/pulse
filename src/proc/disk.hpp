#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <vector>

namespace pulse::proc {
    struct disk_info {
        std::string mount_point;
        std::string filesystem;
        uint64_t total_bytes{};
        uint64_t used_bytes{};
        uint64_t available_bytes{};

        [[nodiscard]] auto used_percent() const -> double {
            if (total_bytes == 0) return 0.0;
            return (static_cast<double>(used_bytes) / static_cast<double>(total_bytes)) * 100.0;
        }
    };
    
    class disk_reader {
        public:
            [[nodiscard]] auto read() -> std::expected<std::vector<disk_info>, std::string>;
    };
}
