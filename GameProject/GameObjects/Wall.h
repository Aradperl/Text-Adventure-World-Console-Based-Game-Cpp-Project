// Wall.h - Wall object

#pragma once

#include "GameObject.h"
#include "GameConfig.h"

class Wall : public GameObject {
public:
    Wall();
    Wall(int x, int y);
    Wall(const Point& pos);
    
    // Always blocks
    bool isBlocking() const override;
    
    // Draw
    void draw() const override;
};
