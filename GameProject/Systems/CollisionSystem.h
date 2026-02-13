// CollisionSystem.h - Handles collision detection

#pragma once

#include "Point.h"
#include "Player.h"
#include "GameBoard.h"
#include "ObjectManager.h"

// System for handling all collision detection in the game
class CollisionSystem {
private:
    GameBoard* board;
    ObjectManager* objects;

public:
    // Constructor
    CollisionSystem();

    // Setup references
    void setBoard(GameBoard* b);
    void setObjectManager(ObjectManager* obj);

    // Collision checks
    bool isBlocked(const Point& p) const;
    bool isWall(const Point& p) const;
    bool isDoor(const Point& p) const;
    bool isGateBar(const Point& p) const;

    // Check collision for player movement (returns true if collision occurred)
    bool checkMovementCollision(const Point& nextPos, const Player* otherPlayer) const;
};
