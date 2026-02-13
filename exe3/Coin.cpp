// Coin.cpp - Collectible coin that contributes to extra lives

#include "Coin.h"
#include "Player.h"
#include "LivesManager.h"
#include <iostream>

// Constructors
Coin::Coin() : GameObject(0, 0, GameConfig::CHAR_COIN, Color::Yellow), collected(false) {}

Coin::Coin(int x, int y) : GameObject(x, y, GameConfig::CHAR_COIN, Color::Yellow), collected(false) {}

Coin::Coin(const Point& pos) : GameObject(pos, GameConfig::CHAR_COIN, Color::Yellow), collected(false) {}

// State
bool Coin::isCollected() const { return collected; }

void Coin::collect() {
    collected = true;
    active = false;
}

void Coin::reset() {
    collected = false;
    active = true;
}

// Interaction
bool Coin::onInteract(Player& player) {
    if (!collected && active) {
        collect();
        LivesManager::addCoin();
        return true;
    }
    return false;
}

// Draw
void Coin::draw() const {
    if (active && !collected) {
        gotoxy(position.getX(), position.getY());
        ::setColor(Color::Yellow);
        std::cout << symbol;
        resetColor();
    }
}
