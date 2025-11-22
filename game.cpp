//
// Created by ארד פרל on 21/11/2025.
//
#include <iostream>
#include "game.h"
#include "InstructionsScreen.h"

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
    
    // Clear any previous error message
    gotoxy(30, 18);
    std::cout << "                                                    ";
}

void Game::handle_menu_input(int choice) {
    switch (choice) {
        case 1:
            start_game();
            break;
        case 8:
            std::cout.flush();  // Flush any pending output before clearing
            InstructionsScreen::display();  // display() already calls clrscr()
            break;
        case 9:
            is_running = false;
            break;
        default:
            gotoxy(30, 18);
            std::cout << "Invalid choice! Please enter 1, 8, or 9.";
            break;
    }
}

void Game::start_game() {
    // TODO: implement the game
    clrscr();
    system("clear");
    gotoxy(1, 1);
    std::cout << "Starting new game... (Not implemented yet). Press any key to return to menu.";
    get_single_char();
}

void Game::run() {
    main_menu();  // Draw menu once at start
    int choice;
    
    while (is_running) {
        choice = get_single_char() - '0';
        
        // Keep menu visible and get input until valid choice is made
        while (choice != 1 && choice != 8 && choice != 9 && is_running) {
            clrscr();
            gotoxy(30, 18);
            std::cout << "Invalid choice! Please enter 1, 8, or 9.";
            choice = get_single_char() - '0';
        }
        
        // Now handle the valid input (or exit)
        if (is_running) {
            handle_menu_input(choice);
            if (choice == 1 || choice == 8) {
                clrscr();
                main_menu();
            }
        }
    }
}


