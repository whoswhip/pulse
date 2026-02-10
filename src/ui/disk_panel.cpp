#include "disk_panel.hpp"

#include <format>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

namespace pulse::ui {

    auto disk_panel::format_bytes(uint64_t bytes) -> std::string {
        constexpr uint64_t gib = 1073741824;
        constexpr uint64_t mib = 1048576;

        if (bytes >= gib) return std::format("{:.1f} GiB", static_cast<double>(bytes) / gib);
        if (bytes >= mib) return std::format("{:.1f} MiB", static_cast<double>(bytes) / mib);

        return std::format("{} B", bytes);
    }

    auto disk_panel::render(const std::vector<proc::disk_info>& disks) -> Element {
        Elements rows;
        for (const auto& d : disks) {
            rows.push_back(
                vbox({
                    hbox({
                        text(std::format("{} [{}]", d.mount_point, d.filesystem)) | dim,
                    }),
                    hbox({
                        gauge(d.used_percent() / 100.0) | flex | color(Color::Magenta),
                        text(std::format(" {} / {}", format_bytes(d.used_bytes), format_bytes(d.total_bytes))),
                    }),
                })
            );
        }

        return vbox({
            text(" Disks") | bold,
            separator(),
            vbox(rows) | flex,
        }) | border;
    }
}
