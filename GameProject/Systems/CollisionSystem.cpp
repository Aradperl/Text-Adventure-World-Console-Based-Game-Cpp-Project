// CollisionSystem.cpp - Handles movement collision checks
// AI usage: Multi-source collision check (AI-assisted)

#include "CollisionSystem.h"
#include "Obstacle.h"

CollisionSystem::CollisionSystem() : board(nullptr), objects(nullptr) {}

// Dependency injection
void CollisionSystem::setBoard(GameBoard* b) { board = b; }
void CollisionSystem::setObjectManager(ObjectManager* obj) { objects = obj; }

// Check if position is blocked
bool CollisionSystem::isBlocked(const Point& p) const {
    if (!board) return true;
    return board->isBlocked(p);
}

bool CollisionSystem::isWall(const Point& p) const {
    if (!board) return false;
    return board->isWall(p);
}

bool CollisionSystem::isDoor(const Point& p) const {
    if (!board) return false;
    return board->isDoor(p);
}

bool CollisionSystem::isGateBar(const Point& p) const {
    if (!board) return false;
    return board->isGateBar(p);
}

// AI-assisted: Comprehensive collision check (walls, gates, obstacles, other player)
bool CollisionSystem::checkMovementCollision(const Point& nextPos, const Player* otherPlayer) const {
    if (!nextPos.isInBounds()) return true;
    if (isWall(nextPos)) return true;

    if (objects) {
        // Closed gates block
        Gate* gate = objects->getGateAt(nextPos);
        if (gate && !gate->getIsOpen()) return true;

        // Obstacles block until pushed
        Obstacle* obstacle = objects->getObstacleAt(nextPos);
        if (obstacle) return true;
    }

    // Can't walk through other player
    if (otherPlayer && otherPlayer->getPosition() == nextPos) return true;

    return false;
}
