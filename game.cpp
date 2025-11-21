//
// Created by ארד פרל on 21/11/2025.
//
#include <iostream>
#include "game.h"

Game::Game() : is_running(true) {

}

void Game::main_menu() {
    clrscr();

    gotoxy(30, 8);
    std::cout << "=======================================";
    gotoxy(30, 9);
    std::cout << "  WELCOME TO TEXT ADVENTURE WORLD  ";
    gotoxy(30, 10);
    std::cout << "=======================================";

    gotoxy(30, 12);
    std::cout << "1. Start New Game";
    gotoxy(30, 13);
    std::cout << "8. Instructions";
    gotoxy(30, 14);
    std::cout << "9. Exit";
    gotoxy(30, 16);
}

void Game::handle_menu_input(int choice) {
    switch (choice) {
        case 1:
            start_game();
            break;
        case 8:
            // TODO: add output for instructions and keys
            break;
        case 9:
            is_running = false;
            break;
        default:
            // TODO: add output for forbidden input
            break;
    }
}

void Game::start_game() {
    // TODO: implement the game
    clrscr();
    gotoxy(1, 1);
    std::cout << "Starting new game... (Not implemented yet). Press any key to return to menu.";
    get_single_char();
}

void Game::run() {
    main_menu();
    handle_menu_input(get_single_char()-'0');
}


