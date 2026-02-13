// MessageDisplay.cpp - Temporary message display with auto-clear timer

#include "MessageDisplay.h"
#include "GameConfig.h"
#include "console.h"
#include <iostream>

// Static silent mode flag
bool MessageDisplay::silentMode = false;

void MessageDisplay::setSilentMode(bool silent) {
    silentMode = silent;
}

// Constructor
MessageDisplay::MessageDisplay() 
    : timer(0), displayY(GameConfig::GAME_HEIGHT + 1) {}

// Set display position
void MessageDisplay::setDisplayY(int y) {
    displayY = y;
}

// Show message
void MessageDisplay::show(const std::string& msg, int duration) {
    message = msg;
    timer = duration;
}

// Clear message
void MessageDisplay::clear() {
    message.clear();
    timer = 0;
}

// Update timer
void MessageDisplay::update() {
    if (timer > 0) {
        timer--;
    }
}

// Check if active
bool MessageDisplay::isActive() const {
    return timer > 0 && !message.empty();
}

// Getters
const std::string& MessageDisplay::getMessage() const {
    return message;
}

int MessageDisplay::getTimer() const {
    return timer;
}

// Draw message
void MessageDisplay::draw() const {
    if (silentMode) return;
    gotoxy(0, displayY);
    if (isActive()) {
        ::setColor(Color::LightYellow);
        std::cout << message;
        // Clear rest of line
        for (size_t i = message.length(); i < 80; i++) {
            std::cout << ' ';
        }
        resetColor();
    } else {
        // Clear line
        std::cout << "                                                                                ";
    }
}
