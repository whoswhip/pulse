#include "proc/cpu.hpp"
#include "ui/cpu_panel.hpp"

#include "proc/memory.hpp"
#include "ui/memory_panel.hpp"

#include "proc/disk.hpp"
#include "ui/disk_panel.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <chrono>
#include <thread>
#include <mutex>

using namespace ftxui;
using namespace pulse;

auto main() -> int {
    auto screen = ScreenInteractive::Fullscreen();
    screen.TrackMouse(false);

    proc::cpu_reader cpu;
    proc::memory_reader memory;
    proc::disk_reader disk;
    ui::cpu_panel cpu_ui;
    ui::mem_panel memory_ui;
    ui::disk_panel disk_ui;

    double cpu_total{};
    std::vector<double> cpu_cores;
    proc::memory_info mem_info;
    std::vector<proc::disk_info> disk_info;
    std::mutex data_mutex;

    std::jthread collector([&](std::stop_token st) {
            while (!st.stop_requested()) {
                auto cpu_result = cpu.read();
                auto mem_result = memory.read();
                auto disk_result = disk.read();
                {
                    std::lock_guard lock(data_mutex);
                    if (cpu_result) {
                        cpu_total = cpu_result->total_percent;
                        cpu_cores = cpu_result->per_core_percent;
                    }
                    if (mem_result) {
                        mem_info = *mem_result;
                    }
                    if (disk_result) {
                        disk_info = *disk_result;
                    }
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            });

    std::jthread refresher([&](std::stop_token st) {
            while (!st.stop_requested()) {
                screen.PostEvent(Event::Custom);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            });



    auto renderer = Renderer([&] {
            std::lock_guard lock(data_mutex);
            return vbox({
                    text("pulse") | bold | center,
                    separator(),
                    hbox({
                        cpu_ui.render(cpu_total, cpu_cores) | flex,
                        vbox({
                            memory_ui.render(mem_info),
                            disk_ui.render(disk_info),
                            }) | flex,
                        }) | flex,
                });
            });
    
    auto component = CatchEvent(renderer, [&](Event event) {
            if (event == Event::Character('q')) {
                screen.Exit();
                return true;
            }
            return false;
            });

    screen.Loop(component);
    return 0;
}
