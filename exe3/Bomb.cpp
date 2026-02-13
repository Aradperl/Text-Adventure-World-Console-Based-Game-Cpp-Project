// Bomb.cpp - Bomb item that explodes after countdown
// Handles collection, activation, countdown, and explosion with wall shielding
// AI usage: Raycast wall shielding (AI-assisted), explosion area calculation (AI-assisted),
//           chain reaction explosion logic (AI-assisted)

#include "Bomb.h"
#include "GameBoard.h"
#include "ObjectManager.h"
#include "Player.h"
#include "GameConfig.h"
#include "console.h"
#include <iostream>
#include <cmath>

// Default constructor - idle bomb
Bomb::Bomb()
    : GameObject(0, 0, '@', Color::LightRed),
    collected(false), activated(false),
    countdownTimer(COUNTDOWN_CYCLES), explosionRadius(EXPLOSION_RADIUS) {
}

// Constructor with position
Bomb::Bomb(int x, int y)
    : GameObject(x, y, '@', Color::LightRed),
    collected(false), activated(false),
    countdownTimer(COUNTDOWN_CYCLES), explosionRadius(EXPLOSION_RADIUS) {
}

// Constructor with Point
Bomb::Bomb(const Point& pos)
    : GameObject(pos, '@', Color::LightRed),
    collected(false), activated(false),
    countdownTimer(COUNTDOWN_CYCLES), explosionRadius(EXPLOSION_RADIUS) {
}

// Mark bomb as collected by player
void Bomb::collect() {
    collected = true;
    active = false;
}

// Plant bomb and start countdown timer
void Bomb::activate(int x, int y) {
    position.setX(x);
    position.setY(y);
    activated = true;
    active = true;
    countdownTimer = COUNTDOWN_CYCLES;
}

// Plant bomb at Point position
void Bomb::activate(const Point& pos) {
    activate(pos.getX(), pos.getY());
}

// Decrement timer, returns true when explosion should occur
bool Bomb::updateCountdown() {
    if (!activated) return false;

    countdownTimer--;
    if (countdownTimer <= 0) {
        return true;
    }
    return false;
}

// AI-assisted: Raycast from bomb to target to detect wall shielding
// Steps along line checking each cell for walls
bool Bomb::isShieldedByWall(const Point& target, GameBoard* board) const {
    if (!board) return false;

    int dx = target.getX() - position.getX();
    int dy = target.getY() - position.getY();

    // Step along the line from bomb to target
    int absDx = (dx < 0) ? -dx : dx;
    int absDy = (dy < 0) ? -dy : dy;
    int steps = (absDx > absDy) ? absDx : absDy;
    if (steps == 0) return false;

    float stepX = (float)dx / steps;
    float stepY = (float)dy / steps;

    // Check each point along the ray for walls
    for (int i = 1; i < steps; i++) {
        int checkX = position.getX() + (int)(stepX * i + 0.5f);
        int checkY = position.getY() + (int)(stepY * i + 0.5f);

        char cell = board->getCell(checkX, checkY);
        if (cell == 'W') {
            return true;
        }
    }

    return false;
}

// AI-assisted: Calculate explosion area excluding wall-shielded cells
std::vector<Point> Bomb::getExplosionArea(GameBoard* board) const {
    std::vector<Point> affected;

    for (int dy = -explosionRadius; dy <= explosionRadius; dy++) {
        for (int dx = -explosionRadius; dx <= explosionRadius; dx++) {
            if (dx == 0 && dy == 0) continue;  // skip bomb position

            int tx = position.getX() + dx;
            int ty = position.getY() + dy;

            // Stay within playable area
            if (tx < 1 || ty < 1 || tx >= GameConfig::GAME_WIDTH - 1 || ty >= GameConfig::GAME_HEIGHT - 1) continue;

            int absDx = (dx < 0) ? -dx : dx;
            int absDy = (dy < 0) ? -dy : dy;
            if (absDx > explosionRadius || absDy > explosionRadius) continue;

            Point target(tx, ty);

            // Only add if not blocked by wall
            if (!isShieldedByWall(target, board)) {
                affected.push_back(target);
            }
        }
    }

    return affected;
}

// Explode without preserve positions
void Bomb::explode(GameBoard* board, ObjectManager* objects, Player* player1, Player* player2) {
    explode(board, objects, player1, player2, std::vector<Point>());
}

// Explode with computed area and preserve list
void Bomb::explode(GameBoard* board, ObjectManager* objects, Player* player1, Player* player2, const std::vector<Point>& preservePositions) {
    std::vector<Point> affected = getExplosionArea(board);
    explode(board, objects, player1, player2, affected, preservePositions);
}

// AI-assisted: Main explosion - destroys objects while preserving chain reaction bombs
// Checks preserve list and active bombs before destroying each cell
void Bomb::explode(GameBoard* board, ObjectManager* objects, Player* player1, Player* player2, const std::vector<Point>& affectedPositions, const std::vector<Point>& preservePositions) {
    if (!activated) return;

    // Player damage is handled by shrapnel in GameLevel for proper tracking
    for (const Point& pos : affectedPositions) {
        // Check if we should preserve this cell (chain reaction bomb there)
        bool shouldPreserve = false;
        for (const Point& preservePos : preservePositions) {
            if (pos == preservePos) {
                shouldPreserve = true;
                break;
            }
        }

        // Safety check for active bombs
        if (!shouldPreserve && objects) {
            for (auto& bomb : objects->getBombs()) {
                if (bomb->getPosition() == pos && bomb->isActive()) {
                    shouldPreserve = true;
                    break;
                }
            }
        }

        // Destroy objects unless preserved
        if (objects && !shouldPreserve) {
            objects->destroyAt(pos);
        }

        // Clear cell but protect doors
        if (board && !shouldPreserve) {
            char cell = board->getCell(pos);
            bool isDoor = (cell >= '1' && cell <= '9');
            if (!isDoor) {
                board->setCell(pos, GameConfig::CHAR_EMPTY);
            }
        }
    }

    // Clear bomb's own cell
    if (board) {
        board->setCell(position, ' ');
    }

    // Bomb is done
    activated = false;
    active = false;
    collected = false;
}

// Reset bomb to idle state for level restart
void Bomb::reset() {
    collected = false;
    activated = false;
    countdownTimer = COUNTDOWN_CYCLES;
    active = true;
}

// Draw bomb with flashing countdown number when activated
void Bomb::draw() const {
    if (!active) return;

    gotoxy(position.getX(), position.getY());

    if (activated) {
        // Alternate colors for urgency
        if (countdownTimer % 2 == 0) {
            ::setColor(Color::LightRed);
        }
        else {
            ::setColor(Color::Yellow);
        }
        std::cout << countdownTimer;
    }
    else {
        ::setColor(Color::LightRed);
        std::cout << symbol;
    }

    resetColor();
}
