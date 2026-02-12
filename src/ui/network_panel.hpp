#pragma once

#include "proc/network.hpp"

#include <ftxui/dom/elements.hpp>
#include <vector>

namespace pulse::ui {
    
    class network_panel {
        public:
            [[nodiscard]] auto render(const std::vector<proc::net_interface>& interfaces) -> ftxui::Element;
        private:
            [[nodiscard]] static auto format_rate(double bytes_per_sec) -> std::string;
    };

}
