#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

auto main() -> int {
    auto screen = ScreenInteractive::Fullscreen();

    auto renderer = Renderer([&] {
            return vbox({
                    text("pulse - system monitor") | bold | center,
                    separator(),
                    text("loading...") | center | flex,
                    }) | border;
            });

    screen.Loop(renderer);
    return 0;
}
