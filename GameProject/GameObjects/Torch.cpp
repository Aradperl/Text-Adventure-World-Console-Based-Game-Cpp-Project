// Torch.cpp - Light source with configurable radius

#include "Torch.h"
#include "Player.h"
#include <iostream>

// Constructors
Torch::Torch() 
    : GameObject(0, 0, GameConfig::CHAR_TORCH, Color::LightRed), 
      lightRadius(5), collected(false) {}

Torch::Torch(int x, int y, int radius) 
    : GameObject(x, y, GameConfig::CHAR_TORCH, Color::LightRed), 
      lightRadius(radius), collected(false) {}

Torch::Torch(const Point& pos, int radius) 
    : GameObject(pos, GameConfig::CHAR_TORCH, Color::LightRed), 
      lightRadius(radius), collected(false) {}

// Properties
int Torch::getLightRadius() const { return lightRadius; }
void Torch::setLightRadius(int radius) { lightRadius = radius; }
bool Torch::isCollected() const { return collected; }

// Collect/drop
void Torch::collect() {
    collected = true;
    active = false;
}

void Torch::drop(const Point& pos) {
    position = pos;
    collected = false;
    active = true;
}

void Torch::reset() {
    collected = false;
    active = true;
}

// Check if point is in light radius
bool Torch::isInLightRadius(const Point& p) const {
    if (!active && !collected) return false;
    
    int dx = p.getX() - position.getX();
    int dy = p.getY() - position.getY();
    return (dx * dx + dy * dy) <= (lightRadius * lightRadius);
}

bool Torch::isInLightRadiusFrom(const Point& center, const Point& p) const {
    int dx = p.getX() - center.getX();
    int dy = p.getY() - center.getY();
    return (dx * dx + dy * dy) <= (lightRadius * lightRadius);
}

// Interaction
bool Torch::onInteract(Player& player) {
    if (!collected && active) {
        collect();
        player.pickupItem(GameConfig::CHAR_TORCH);
        return true;
    }
    return false;
}

// Draw
void Torch::draw() const {
    if (active && !collected) {
        gotoxy(position.getX(), position.getY());
        ::setColor(Color::LightRed);
        std::cout << symbol;
        resetColor();
    }
}
