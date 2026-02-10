#pragma once

#include "proc/disk.hpp"

#include <ftxui/dom/elements.hpp>
#include <vector>

namespace pulse::ui {

    class disk_panel {
        public:
            [[nodiscard]] auto render(const std::vector<proc::disk_info>& disks) -> ftxui::Element;

        private:
            [[nodiscard]] static auto format_bytes(uint64_t bytes) -> std::string;
    };
}
