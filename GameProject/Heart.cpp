// Heart.cpp - Collectible heart that grants extra life

#include "Heart.h"

// Constructors (ASCII 3 = heart symbol ♥)
Heart::Heart() 
    : GameObject(0, 0, 3, Color::LightRed),
      spawnTimer(SPAWN_DURATION) {
}

Heart::Heart(int x, int y) 
    : GameObject(x, y, 3, Color::LightRed),
      spawnTimer(SPAWN_DURATION) {
}

// Reset spawn timer
void Heart::resetTimer() {
    spawnTimer = SPAWN_DURATION;
}

// Update timer and check if expired
bool Heart::updateTimer() {
    if (spawnTimer > 0) {
        spawnTimer--;
    }
    return spawnTimer <= 0;
}

// Get remaining time
int Heart::getTimeRemaining() const {
    return spawnTimer;
}

// Player can walk over it
bool Heart::isBlocking() const {
    return false;
}
