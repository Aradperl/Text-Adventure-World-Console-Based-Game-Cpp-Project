// LightSwitch.cpp - Room lighting toggle in dark rooms

#include "LightSwitch.h"
#include "Player.h"
#include <iostream>

// Constructors
LightSwitch::LightSwitch() : GameObject(0, 0, 'S', Color::Yellow), isOn(false) {}

LightSwitch::LightSwitch(int x, int y, bool startOn)
    : GameObject(x, y, 'S', startOn ? Color::LightGreen : Color::Yellow), isOn(startOn) {
}

LightSwitch::LightSwitch(const Point& pos, bool startOn)
    : GameObject(pos, 'S', startOn ? Color::LightGreen : Color::Yellow), isOn(startOn) {
}

// State
bool LightSwitch::getIsOn() const { return isOn; }

void LightSwitch::toggle() {
    isOn = !isOn;
}

void LightSwitch::turnOn() {
    isOn = true;
}

void LightSwitch::turnOff() {
    isOn = false;
}

void LightSwitch::reset() {
    isOn = false;
    active = true;
}

// Interaction
bool LightSwitch::onInteract(Player& player) {
    toggle();
    return true;
}

// Draw
void LightSwitch::draw() const {
    if (active) {
        gotoxy(position.getX(), position.getY());
        if (isOn) {
            ::setColor(Color::LightGreen);
        }
        else {
            ::setColor(Color::Yellow);
        }
        std::cout << symbol;
        resetColor();
    }
}