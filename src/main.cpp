#include "proc/cpu.hpp"
#include "ui/cpu_panel.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <atomic>
#include <chrono>
#include <thread>

using namespace ftxui;
using namespace pulse;

auto main() -> int {
    auto screen = ScreenInteractive::Fullscreen();
    
    proc::cpu_reader cpu;
    ui::cpu_panel cpu_ui;

    double cpu_total{};
    std::vector<double> cpu_cores;
    std::mutex data_mutex;
    std::atomic<bool> running{true};

    std::jthread collector([&](std::stop_token st) {
            while (!st.stop_requested()) {
                if (auto result = cpu.read()) {
                    std::lock_guard lock(data_mutex);
                    cpu_total = result->total_percent;
                    cpu_cores = result->per_core_percent;
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
                    cpu_ui.render(cpu_total, cpu_cores) | flex,
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
