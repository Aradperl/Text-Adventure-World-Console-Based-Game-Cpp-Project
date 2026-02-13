// Torch.h - Torch that provides light

#pragma once

#include "GameObject.h"
#include "GameConfig.h"

class Player;

class Torch : public GameObject {
private:
    int lightRadius;
    bool collected;

public:
    Torch();
    Torch(int x, int y, int radius = 3);
    Torch(const Point& pos, int radius = 3);
    
    // Properties
    int getLightRadius() const;
    void setLightRadius(int radius);
    bool isCollected() const;
    
    // Collect/drop
    void collect();
    void drop(const Point& pos);
    void reset();
    
    // Light radius checks
    bool isInLightRadius(const Point& p) const;
    bool isInLightRadiusFrom(const Point& center, const Point& p) const;
    
    // Interaction
    bool onInteract(Player& player) override;
    
    // Draw
    void draw() const override;
};
