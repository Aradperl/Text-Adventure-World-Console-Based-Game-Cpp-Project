#include <iostream>
#include "console.h"
#include "game.h"

int main() {
    init_console();
    Game game;
    game.run();
    cleanup_console();
    return 0;
}

