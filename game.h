//
// Created by ארד פרל on 21/11/2025.
//
#pragma once
#include "console.h"

class Game {
    bool is_running;
    void main_menu();
    void start_game();
    void handle_menu_input(int choice);

    public:
    Game();
    void run();

};




