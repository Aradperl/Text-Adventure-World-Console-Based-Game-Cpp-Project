// Key.cpp - Collectible key for opening gates

#include "Key.h"
#include "Player.h"
#include <iostream>

// Constructors
Key::Key() : GameObject(0, 0, GameConfig::CHAR_KEY, Color::Yellow), collected(false) {}

Key::Key(int x, int y) : GameObject(x, y, GameConfig::CHAR_KEY, Color::Yellow), collected(false) {}

Key::Key(const Point& pos) : GameObject(pos, GameConfig::CHAR_KEY, Color::Yellow), collected(false) {}

// State
bool Key::isCollected() const { return collected; }

void Key::collect() {
    collected = true;
    active = false;
}

void Key::reset() {
    collected = false;
    active = true;
}

// Interaction
bool Key::onInteract(Player& player) {
    if (!collected && active) {
        collect();
        player.pickupItem(GameConfig::CHAR_KEY);
        return true;
    }
    return false;
}

// Draw
void Key::draw() const {
    if (active && !collected) {
        gotoxy(position.getX(), position.getY());
        ::setColor(Color::Yellow);
        std::cout << symbol;
        resetColor();
    }
}
