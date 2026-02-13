// LivesManager.h - Shared lives for both players

#pragma once

class LivesManager {
private:
    static int sharedLives;
    static int coinsCollected;
    static const int INITIAL_LIVES = 4;
    static const int COINS_PER_LIFE = 50;

public:
    static int getLives();
    static bool hasLivesRemaining();
    static void resetLives();
    static void addLife();
    static void subtractLife();

    // Coin management
    static int getCoins();
    static void addCoin();
    static void resetCoins();
};
