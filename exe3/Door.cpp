// Door.cpp - Level exit doors requiring keys

#include "Door.h"
#include "Player.h"
#include <iostream>

// Default constructor
Door::Door() 
    : GameObject(0, 0, '1', Color::Magenta), 
      doorNum('1'), keysRequired(1), keysUsed(0), isOpen(false), targetScreen(-1) {}

// Constructor with coordinates
Door::Door(int x, int y, char num, int keys, int target)
    : GameObject(x, y, num, Color::Magenta),
      doorNum(num), keysRequired(keys), keysUsed(0), isOpen(false), targetScreen(target) {}

// Constructor with Point
Door::Door(const Point& pos, char num, int keys, int target)
    : GameObject(pos, num, Color::Magenta),
      doorNum(num), keysRequired(keys), keysUsed(0), isOpen(false), targetScreen(target) {}

// Getters
char Door::getDoorNum() const { return doorNum; }
int Door::getKeysRequired() const { return keysRequired; }
int Door::getKeysUsed() const { return keysUsed; }
int Door::getTargetScreen() const { return targetScreen; }
bool Door::isDoorOpen() const { return isOpen; }

// Set target screen for door
void Door::setTargetScreen(int screen) { targetScreen = screen; }

// Try to use a key on the door
bool Door::useKey() {
    if (!isOpen) {
        keysUsed++;
        if (keysUsed >= keysRequired) {
            open();
            return true;
        }
    }
    return false;
}

// Open the door
void Door::open() {
    isOpen = true;
    symbol = ' ';
}

// Reset door to initial state
void Door::reset() {
    isOpen = false;
    keysUsed = 0;
    symbol = doorNum;
    active = true;
}

// Check if door is blocking movement
bool Door::isBlocking() const { return !isOpen; }

// Handle player interaction with door
bool Door::onInteract(Player& player) {
    if (!isOpen && player.hasKey()) {
        player.disposeItem();
        return useKey();
    }
    return false;
}

// Draw the door on screen
void Door::draw() const {
    if (active && !isOpen) {
        gotoxy(position.getX(), position.getY());
        ::setColor(Color::Magenta);
        std::cout << doorNum;
        resetColor();
    }
}
