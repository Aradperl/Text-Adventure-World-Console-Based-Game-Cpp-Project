#include "console.h"
#include "Screen.h"

void Screen::draw() const {
    set_color(Color::Yellow);
    for (size_t i = 0; i < MAX_Y; ++i) {
        gotoxy(0, i);
        std::cout << screen[i] << std::flush;
    }
    reset_color();
}
