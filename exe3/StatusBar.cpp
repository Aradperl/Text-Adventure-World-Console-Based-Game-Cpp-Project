// StatusBar.cpp - Bottom HUD showing lives, inventory, and level info

#include "StatusBar.h"
#include "GameConfig.h"
#include "LivesManager.h"
#include "console.h"
#include <iostream>

// Static silent mode flag
bool StatusBar::silentMode = false;

void StatusBar::setSilentMode(bool silent) {
    silentMode = silent;
}

// Constructor
StatusBar::StatusBar() 
    : levelNumber(1), isDarkRoom(false), isRoomLit(false),
      player1(nullptr), player2(nullptr), bombCountdown(0) {}

// Setup
void StatusBar::setLevelNumber(int num) {
    levelNumber = num;
}

void StatusBar::setPlayers(const Player* p1, const Player* p2) {
    player1 = p1;
    player2 = p2;
}

void StatusBar::setDarkRoomStatus(bool dark, bool lit) {
    isDarkRoom = dark;
    isRoomLit = lit;
}

void StatusBar::setBombCountdown(int ticks) {
    bombCountdown = ticks;
}

// Draw status bar
void StatusBar::draw() const {
    if (silentMode) return;
    
    // Level
    gotoxy(0, GameConfig::GAME_HEIGHT);
    ::setColor(Color::White);
    std::cout << "Lv" << levelNumber << " ";
    
    // Player 1 inventory
    gotoxy(5, GameConfig::GAME_HEIGHT);
    ::setColor(Color::LightGreen);
    std::cout << "P1:";
    if (player1 && player1->hasKey()) {
        ::setColor(Color::Yellow);
        std::cout << "Key  ";
    } else if (player1 && player1->hasTorch()) {
        ::setColor(Color::LightRed);
        std::cout << "Torch";
    } else if (player1 && player1->hasBomb()) {
        ::setColor(Color::LightRed);
        std::cout << "Bomb ";
    } else {
        std::cout << "Empty";
    }
    
    // Player 2 inventory
    gotoxy(18, GameConfig::GAME_HEIGHT);
    ::setColor(Color::LightCyan);
    std::cout << "P2:";
    if (player2 && player2->hasKey()) {
        ::setColor(Color::Yellow);
        std::cout << "Key  ";
    } else if (player2 && player2->hasTorch()) {
        ::setColor(Color::LightRed);
        std::cout << "Torch";
    } else if (player2 && player2->hasBomb()) {
        ::setColor(Color::LightRed);
        std::cout << "Bomb ";
    } else {
        std::cout << "Empty";
    }
    
    // Lives and coins (shared bank)
    gotoxy(31, GameConfig::GAME_HEIGHT);
    ::setColor(Color::LightRed);
    std::cout << "Lives:" << LivesManager::getLives() << " ";
    gotoxy(42, GameConfig::GAME_HEIGHT);
    ::setColor(Color::Yellow);
    std::cout << "Coins:" << LivesManager::getCoins() << "/50 ";

    // Dark room status
    gotoxy(55, GameConfig::GAME_HEIGHT);
    if (isDarkRoom) {
        if (isRoomLit) {
            ::setColor(Color::LightGreen);
            std::cout << "[LIT]  ";
        } else {
            ::setColor(Color::Magenta);
            std::cout << "[DARK] ";
        }
    } else {
        std::cout << "       ";
    }

    // Menu hint
    gotoxy(65, GameConfig::GAME_HEIGHT);
    ::setColor(Color::Gray);
    std::cout << "ESC=Menu";

    // Bomb countdown (if any)
    gotoxy(76, GameConfig::GAME_HEIGHT);
    if (bombCountdown > 0) {
        ::setColor(Color::LightRed);
        std::cout << "Bomb:" << bombCountdown << " ";
    } else {
        std::cout << "       ";
    }
    
    resetColor();
}
