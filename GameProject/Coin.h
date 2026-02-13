// Coin.h - Coin game object

#pragma once

#include "GameObject.h"
#include "GameConfig.h"

class Player;

class Coin : public GameObject {
private:
    bool collected;

public:
    Coin();
    Coin(int x, int y);
    Coin(const Point& pos);

    // State
    bool isCollected() const;
    void collect();
    void reset();

    // Interaction
    bool onInteract(Player& player) override;

    // Draw
    void draw() const override;
};
