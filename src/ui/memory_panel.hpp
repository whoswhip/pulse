#pragma once

#include <proc/memory.hpp>
#include <ftxui/dom/elements.hpp>

namespace pulse::ui {
    class mem_panel {
        public:
            [[nodiscard]] auto render(proc::memory_info& info) -> ftxui::Element;
        private:
            [[nodiscard]] static auto format_kb(uint64_t kb) -> std::string;
    };
}
