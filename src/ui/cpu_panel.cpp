#include "cpu_panel.hpp"

#include <ftxui/dom/elements.hpp>
#include <format>

using namespace ftxui;

namespace pulse::ui {

    auto cpu_panel::render(double total, const std::vector<double>& per_core) -> Element {
        m_history.push_back(total / 100.0);
        if (m_history.size() > m_history_size) {
            m_history.erase(m_history.begin());
        }

        Elements core_guages;
        for (size_t i = 0; i < per_core.size(); ++i) {
            core_guages.push_back(
                    hbox({
                        text(std::format("core {:2d} ", i)) | dim,
                        gauge(per_core[i] / 100.0) | flex | color(Color::Cyan),
                        text(std::format(" {:5.1f}%", per_core[i])),
                        })
                    );
        }

        return vbox({
                text(std::format(" CPU - {:.1f}%", total)) | bold,
                separator(),
                graph([this](int width, int height) {
                        std::vector<int> output(width);
                        auto start = m_history.size() > static_cast<size_t>(width) ? m_history.size() - width : 0;
                        for (int i = 0; i < width && (start + i) < m_history.size(); ++i) {
                            output[i] = static_cast<int>(m_history[start + i] * height);
                        }
                        return output;
                        }) | flex | color(Color::Cyan),
                separator(),
                vbox(core_guages),
                }) | border;
    }
}
