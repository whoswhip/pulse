#include "memory.hpp"

#include <fstream>
#include <string>

namespace pulse::proc {

    auto memory_reader::read() -> std::expected<memory_info, std::string> {
        std::ifstream file("/proc/meminfo");
        if (!file.is_open()) {
            return std::unexpected("failed to open /proc/meminfo");
        }

        memory_info info;
        std::string key;
        uint64_t value;
        std::string unit;

        while (file >> key >> value) {
            file >> unit;

            if (key == "MemTotal:") info.total_kb = value;
            else if (key == "MemAvailable:") info.available_kb = value;
            else if (key == "Buffers:") info.buffers_kb = value;
            else if (key == "Cached:") info.cached_kb = value;
            else if (key == "SwapTotal:") info.swap_total_kb = value;
            else if (key == "SwapFree:"){
                info.swap_used_kb = info.swap_total_kb - value;
            }
        }

        info.used_kb = info.total_kb - info.available_kb;
        return info;
    }
}
