#pragma once

#include <ftxui/dom/elements.hpp>
#include <vector>

namespace pulse::ui {
    
    class cpu_panel {
        public:
            [[nodiscard]] auto render(double total, const std::vector<double>& per_core) -> ftxui::Element;
        private:
            static constexpr int m_history_size = 60;
            std::vector<double> m_history;
    };

}
