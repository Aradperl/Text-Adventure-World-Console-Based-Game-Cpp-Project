// BaseScreen.cpp - Abstract base class for all game screens

#include "BaseScreen.h"

// Default constructor
BaseScreen::BaseScreen() : active(false) {}

// Initialize screen resources
void BaseScreen::init() {}

// Activate screen
void BaseScreen::enter() { active = true; }

// Deactivate screen
void BaseScreen::exit() { active = false; }

// Reset to initial state
void BaseScreen::reset() {}

// Check active status
bool BaseScreen::isActive() const { return active; }
