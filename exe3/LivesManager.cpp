// LivesManager.cpp - Shared lives and coin pool for both players

#include "LivesManager.h"

// Initialize shared lives and coins
int LivesManager::sharedLives = LivesManager::INITIAL_LIVES;
int LivesManager::coinsCollected = 0;

// Lives management
int LivesManager::getLives() { return sharedLives; }
bool LivesManager::hasLivesRemaining() { return sharedLives > 0; }
void LivesManager::resetLives() {
    sharedLives = INITIAL_LIVES;
    coinsCollected = 0;
}

void LivesManager::addLife() {
    sharedLives++;
}

void LivesManager::subtractLife() {
    if (sharedLives > 0) {
        sharedLives--;
    }
}

// Coin management
int LivesManager::getCoins() { return coinsCollected; }

void LivesManager::addCoin() {
    coinsCollected++;
    // Check if we should convert coins to life
    if (coinsCollected >= COINS_PER_LIFE) {
        coinsCollected -= COINS_PER_LIFE;
        addLife();
    }
}

void LivesManager::resetCoins() { coinsCollected = 0; }
