// Wall.cpp - Impassable wall tile

#include "Wall.h"
#include <iostream>

// Constructors
Wall::Wall() : GameObject(0, 0, GameConfig::CHAR_WALL, Color::Gray) {}

Wall::Wall(int x, int y) : GameObject(x, y, GameConfig::CHAR_WALL, Color::Gray) {}

Wall::Wall(const Point& pos) : GameObject(pos, GameConfig::CHAR_WALL, Color::Gray) {}

// Walls always block
bool Wall::isBlocking() const { return true; }

// Draw
void Wall::draw() const {
    if (active) {
        gotoxy(position.getX(), position.getY());
        ::setColor(Color::Gray);
        std::cout << symbol;
        resetColor();
    }
}
