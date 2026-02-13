// Switch.cpp - Toggle switch linked to a specific gate

#include "Switch.h"
#include "Player.h"
#include <iostream>

// Constructors
Switch::Switch() : GameObject(0, 0, '\\', Color::Gray), isOn(false), linkedGate('\0') {}

Switch::Switch(int x, int y, char gateNum, bool startOn)
    : GameObject(x, y, startOn ? '/' : '\\', startOn ? Color::LightGreen : Color::Gray),
    isOn(startOn), linkedGate(gateNum) {
}

Switch::Switch(const Point& pos, char gateNum, bool startOn)
    : GameObject(pos, startOn ? '/' : '\\', startOn ? Color::LightGreen : Color::Gray),
    isOn(startOn), linkedGate(gateNum) {
}

// State
bool Switch::getIsOn() const { return isOn; }

void Switch::toggle() {
    isOn = !isOn;
    symbol = isOn ? '/' : '\\';
}

void Switch::turnOn() {
    isOn = true;
}

void Switch::turnOff() {
    isOn = false;
}

void Switch::reset() {
    isOn = false;
    symbol = '\\';
    active = true;
}

// Gate link
char Switch::getLinkedGate() const { return linkedGate; }

// Interaction
bool Switch::onInteract(Player& player) {
    toggle();
    return true;
}

// Draw
void Switch::draw() const {
    if (active) {
        gotoxy(position.getX(), position.getY());
        if (isOn) {
            ::setColor(Color::LightGreen);
        }
        else {
            ::setColor(Color::Gray);
        }
        std::cout << symbol;
        resetColor();
    }
}