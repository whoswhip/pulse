#include "network_panel.hpp"

#include <format>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

namespace pulse::ui {

    auto network_panel::format_rate(double bytes_per_sec) -> std::string {
        constexpr double gib = 1073741824.0;
        constexpr double mib = 1048576.0;
        constexpr double kib = 1024.0;

        if (bytes_per_sec >= gib) return std::format("{:.1f} GiB/s", bytes_per_sec / gib); 
        if (bytes_per_sec >= mib) return std::format("{:.1f} MiB/s", bytes_per_sec / mib); 
        if (bytes_per_sec >= kib) return std::format("{:.1f} KiB/s", bytes_per_sec / kib);
        return std::format("{:.0f} B/s", bytes_per_sec);
    }

    auto network_panel::render(const std::vector<proc::net_interface>& interfaces) -> Element {
        Elements rows;
        for (const auto& iface : interfaces) {
            rows.push_back(
                hbox({
                    text(std::format("{:<12}", iface.name)) | bold,
                    text(" ↓ ") | color(Color::Green),
                    text(std::format("{:>12}", format_rate(iface.rx_rate_bytes_sec))),
                    text(" ↑ ") | color(Color::Red),
                    text(std::format("{:>12}", format_rate(iface.tx_rate_bytes_sec))),
                })
            );
        }

        return vbox({
            text(" Network") | bold,
            separator(),
            vbox(rows) | flex,
        }) | border;
    }
}
