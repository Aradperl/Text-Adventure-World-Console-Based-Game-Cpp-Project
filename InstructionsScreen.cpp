
#include "InstructionsScreen.h"
#include <iostream>

void InstructionsScreen::display() {
    clrscr();
    system("clear");

    gotoxy(30, 3);
    std::cout << "--- GAME INSTRUCTIONS ---";

    gotoxy(10, 5);
    std::cout << "PLAYER 1 (@):";
    gotoxy(10, 6);
    std::cout << "  'W' - Move Up";
    gotoxy(10, 7);
    std::cout << "  'X' - Move Down";
    gotoxy(10, 8);
    std::cout << "  'S' - Stay In Place";
    gotoxy(10, 9);
    std::cout << "  'A' - Move Left";
    gotoxy(10, 10);
    std::cout << "  'D' - Move Right";


    gotoxy(35, 5);
    std::cout << "PLAYER 2 (&):";
    gotoxy(35, 6);
    std::cout << "  'I' - Move Up";
    gotoxy(35, 7);
    std::cout << "  'M' - Move Down";
    gotoxy(35, 8);
    std::cout << "  'K' - Stay In Place";
    gotoxy(35, 9);
    std::cout << "  'J' - Move Left";
    gotoxy(35, 10);
    std::cout << "  'L' - Move Right";


    gotoxy(60, 5);
    std::cout << "GAME OBJECTIVES:";
    gotoxy(60, 6);
    std::cout << "  W - Wall (Cannot Pass)";
    gotoxy(60, 7);
    std::cout << "  * - Obstacle (Cannot Pass)";
    gotoxy(60, 8);
    std::cout << "  K - Key (Collect to open doors)";
    gotoxy(60, 9);
    std::cout << "  A Digit 1-9 - Door (Requires Key)";
    gotoxy(60, 10);
    std::cout << "  # - Spring (Jumps over space)";
    gotoxy(60, 11);
    std::cout << "  ? - Riddle (Jumps over space)";

    gotoxy(30, 20);
    std::cout << "Press any key to return to the main menu...";

    get_single_char();
}