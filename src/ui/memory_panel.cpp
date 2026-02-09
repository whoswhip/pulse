#include "memory_panel.hpp"
#include "proc/memory.hpp"

#include <ftxui/dom/elements.hpp>
#include <format>

using namespace ftxui;

namespace pulse::ui {

    auto mem_panel::format_kb(uint64_t kb) -> std::string {
        if (kb >= 1048576) return std::format("{:.1f} GiB", static_cast<double>(kb) / 1048576.0);
        if (kb >= 1024) return std::format("{:.1f} MiB", static_cast<double>(kb) / 1024.0);
        return std::format("{} KiB", kb);
    }

    auto mem_panel::render(proc::memory_info& info) -> Element {
        return vbox({
                text(" Memory") | bold,
                separator(),
                hbox({
                        text("RAM  ") | dim,
                        gauge(info.used_percent() / 100.0) | flex | color(Color::Green),
                        text(std::format(" {} / {}", format_kb(info.used_kb), format_kb(info.total_kb)))
                }),
                hbox({
                        text("Swap ") | dim,
                        gauge(info.swap_percent() / 100.0) | flex | color(Color::Yellow),
                        text(std::format(" {} / {}", format_kb(info.swap_used_kb), format_kb(info.swap_total_kb)))
                }),
                separator(),
                hbox({
                    text(std::format("Buffer: {}  Cached: {}", format_kb(info.buffers_kb), format_kb(info.cached_kb)))
                }),
                }) | border;
    }
}
