// Bomb.h - Explosive collectible item

#pragma once

#include "GameObject.h"
#include "Point.h"
#include <vector>

class GameBoard;
class ObjectManager;
class Player;

class Bomb : public GameObject {
private:
    bool collected;
    bool activated;
    int countdownTimer;
    int explosionRadius;
    
    static const int COUNTDOWN_CYCLES = 5;  // 5 game cycles (5->1)
    static const int EXPLOSION_RADIUS = 4;  // Explosion reaches 4 cells in each direction

public:
    Bomb();
    Bomb(int x, int y);
    Bomb(const Point& pos);
    
    static const char BOMB_CHAR = '@';
    
    // State queries
    bool isCollected() const { return collected; }
    bool isActivated() const { return activated; }
    bool isActiveCountdown() const { return activated; }
    bool isIdle() const { return !collected && !activated && active; }
    int getCountdown() const { return countdownTimer; }
    int getExplosionRadius() const { return explosionRadius; }
    
    // Actions
    void collect();
    void activate(int x, int y);
    void activate(const Point& pos);  // Overload for Point
    
    // Update countdown - returns true when explosion happens
    bool updateCountdown();
    
    // Get positions affected by explosion (respecting wall shielding)
    std::vector<Point> getExplosionArea(GameBoard* board) const;
    
    // Check if a point is shielded from explosion by walls
    bool isShieldedByWall(const Point& target, GameBoard* board) const;
    
    // Perform explosion
    void explode(GameBoard* board, ObjectManager* objects, Player* player1, Player* player2);
    void explode(GameBoard* board, ObjectManager* objects, Player* player1, Player* player2, const std::vector<Point>& preservePositions);
    void explode(GameBoard* board, ObjectManager* objects, Player* player1, Player* player2, const std::vector<Point>& affectedPositions, const std::vector<Point>& preservePositions);
    
    // Reset bomb to original state
    void reset();
    void resetBomb() { reset(); }
    
    // Draw
    void draw() const override;
};
