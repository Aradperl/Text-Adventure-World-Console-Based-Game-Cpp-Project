// Key.h - Key game object

#pragma once

#include "GameObject.h"
#include "GameConfig.h"

class Player;

class Key : public GameObject {
private:
    bool collected;

public:
    Key();
    Key(int x, int y);
    Key(const Point& pos);
    
    // State
    bool isCollected() const;
    void collect();
    void reset();
    
    // Interaction
    bool onInteract(Player& player) override;
    
    // Draw
    void draw() const override;
};
