// InstructionsScreen.cpp - Game controls and element reference

#include "InstructionsScreen.h"
#include "GameConfig.h"
#include <conio.h>
#include <iostream>

// Constructor
InstructionsScreen::InstructionsScreen()
    : waitingForKey(true), result(ScreenResult::None), rendered(false) {
}

// Enter screen
void InstructionsScreen::enter() {
    BaseScreen::enter();
    waitingForKey = true;
    result = ScreenResult::None;
    rendered = false;
}

// Handle input
void InstructionsScreen::handleInput() {
    if (waitingForKey && _kbhit()) {
        _getch();
        result = ScreenResult::ExitToMenu;
        waitingForKey = false;
    }
}

// Update (no dynamic state)
void InstructionsScreen::update() {
}

// Render instructions
void InstructionsScreen::render() {
    if (rendered) return;
    rendered = true;

    clrscr();

    // Title
    ::setColor(Color::LightYellow);
    gotoxy(25, 1);
    std::cout << "===== GAME INSTRUCTIONS =====";
    resetColor();

    // Player 1 controls
    ::setColor(Color::LightGreen);
    gotoxy(5, 4);
    std::cout << "PLAYER 1 (" << GameConfig::CHAR_PLAYER1 << "):";
    resetColor();
    gotoxy(5, 5);
    std::cout << "  W - Move Up";
    gotoxy(5, 6);
    std::cout << "  X - Move Down";
    gotoxy(5, 7);
    std::cout << "  A - Move Left";
    gotoxy(5, 8);
    std::cout << "  D - Move Right";
    gotoxy(5, 9);
    std::cout << "  S - Stay (Stop)";
    gotoxy(5, 10);
    std::cout << "  E - Dispose Item";

    // Player 2 controls
    ::setColor(Color::LightCyan);
    gotoxy(30, 4);
    std::cout << "PLAYER 2 (" << GameConfig::CHAR_PLAYER2 << "):";
    resetColor();
    gotoxy(30, 5);
    std::cout << "  I - Move Up";
    gotoxy(30, 6);
    std::cout << "  M - Move Down";
    gotoxy(30, 7);
    std::cout << "  J - Move Left";
    gotoxy(30, 8);
    std::cout << "  L - Move Right";
    gotoxy(30, 9);
    std::cout << "  K - Stay (Stop)";
    gotoxy(30, 10);
    std::cout << "  O - Dispose Item";

    // Game elements
    ::setColor(Color::LightMagenta);
    gotoxy(55, 4);
    std::cout << "GAME ELEMENTS:";
    resetColor();
    gotoxy(55, 5);
    std::cout << "  W - Wall";
    gotoxy(55, 6);
    ::setColor(Color::Yellow);
    std::cout << "  K";
    resetColor();
    std::cout << " - Key";
    gotoxy(55, 7);
    std::cout << "  1-9 - Doors";
    gotoxy(55, 8);
    ::setColor(Color::LightRed);
    std::cout << "  !";
    resetColor();
    std::cout << " - Torch (lights area)";
    gotoxy(55, 9);
    ::setColor(Color::Yellow);
    std::cout << "  S";
    resetColor();
    std::cout << " - LightSwitch OFF";
    gotoxy(55, 10);
    ::setColor(Color::LightGreen);
    std::cout << "  S";
    resetColor();
    std::cout << " - LightSwitch ON";
    gotoxy(55, 11);
    ::setColor(Color::White);
    std::cout << "  \\";
    resetColor();
    std::cout << " - Switch OFF";
    gotoxy(55, 12);
    ::setColor(Color::LightGreen);
    std::cout << "  /";
    resetColor();
    std::cout << " - Switch ON (linked to gate)";
    
    gotoxy(55, 13);
    ::setColor(Color::LightRed);
    std::cout << "  @";
    resetColor();
    std::cout << " - Bomb (explodes after 5s)";
    
    gotoxy(55, 14);
    ::setColor(Color::White);
    std::cout << "  *";
    resetColor();
    std::cout << " - Obstacle (pushable)";
    
    gotoxy(55, 15);
    ::setColor(Color::LightCyan);
    std::cout << "  #";
    resetColor();
    std::cout << " - Spring (launches player)";

    // Objectives
    ::setColor(Color::Yellow);
    gotoxy(5, 17);
    std::cout << "OBJECTIVE:";
    resetColor();
    gotoxy(5, 18);
    std::cout << "Find a KEY to open the GATE, or activate all SWITCHES linked to it!";
    gotoxy(5, 19);
    std::cout << "BOTH players must stand on the door to go to next level!";
    gotoxy(5, 20);
    std::cout << "Some rooms are DARK - use a TORCH or find a LIGHT SWITCH!";
    gotoxy(5, 21);
    std::cout << "Push OBSTACLES together (2 players = 2 blocks). Springs boost your force!";

    ::setColor(Color::Gray);
    gotoxy(25, 23);
    std::cout << "Press any key to return to menu...";
    resetColor();

    std::cout.flush();
}

ScreenResult InstructionsScreen::getResult() const {
    return result;
}

void InstructionsScreen::reset() {
    waitingForKey = true;
    result = ScreenResult::None;
    rendered = false;
}