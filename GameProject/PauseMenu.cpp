// PauseMenu.cpp - In-game pause overlay with save/menu options

#include "PauseMenu.h"
#include "GameConfig.h"
#include "console.h"
#include <iostream>

// Constructor
PauseMenu::PauseMenu() : isPaused(false) {}

// State
bool PauseMenu::getIsPaused() const {
    return isPaused;
}

void PauseMenu::pause() {
    isPaused = true;
}

void PauseMenu::resume() {
    isPaused = false;
}

void PauseMenu::toggle() {
    isPaused = !isPaused;
}

void PauseMenu::reset() {
    isPaused = false;
}

// Draw pause overlay
void PauseMenu::draw() const {
    if (!isPaused) return;

    int centerX = GameConfig::GAME_WIDTH / 2;
    int centerY = GameConfig::GAME_HEIGHT / 2;

    ::setColor(Color::White);

    gotoxy(centerX - 10, centerY - 3);
    std::cout << "===================";
    gotoxy(centerX - 10, centerY - 2);
    std::cout << "|   GAME PAUSED   |";
    gotoxy(centerX - 10, centerY - 1);
    std::cout << "===================";
    gotoxy(centerX - 10, centerY);
    std::cout << "| ESC - Resume    |";
    gotoxy(centerX - 10, centerY + 1);
    std::cout << "|  S  - Save Game |";
    gotoxy(centerX - 10, centerY + 2);
    std::cout << "|  H  - Main Menu |";
    gotoxy(centerX - 10, centerY + 3);
    std::cout << "===================";

    resetColor();
}
