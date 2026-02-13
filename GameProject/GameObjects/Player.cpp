// Player.cpp - Player character controls and state
// Handles movement, inventory, and spring momentum

#include "Player.h"
#include <iostream>

// Static silent mode for automated testing
bool Player::silentMode = false;

void Player::setSilentMode(bool silent) {
    silentMode = silent;
}

// Delegate to shared life pool
void Player::addLife() { LivesManager::addLife(); }
void Player::subtractLife() { LivesManager::subtractLife(); }

Player::Player(int x, int y, char sym, Color col)
    : position(x, y), currentDirection(Direction::STAY), 
      symbol(sym), color(col), heldItem(0), isMoving(false),
      verticalMoveCounter(0), springDirection(Direction::STAY),
      springSpeed(0), springDuration(0), hasSpringMomentum(false),
      lastMoveDirection(Direction::STAY), lastMoveAge(999) {}

// Position access
Point Player::getPosition() const { return position; }
void Player::setPosition(const Point& pos) { position = pos; }
void Player::setPosition(int x, int y) { position.set(x, y); }

Direction Player::getDirection() const { return currentDirection; }

void Player::setDirection(Direction dir) {
    currentDirection = dir;
    isMoving = (dir != Direction::STAY);
    verticalMoveCounter = 0;
    
    // Track for cooperative pushing
    if (dir != Direction::STAY) {
        lastMoveDirection = dir;
        lastMoveAge = 0;
    }
}

bool Player::getIsMoving() const { return isMoving; }

void Player::stop() {
    currentDirection = Direction::STAY;
    isMoving = false;
}

// AI-assisted: Vertical movement throttled for visual balance (every 2 frames)
bool Player::canMoveThisFrame() {
    if (!isMoving) return false;
    
    if (currentDirection == Direction::UP || currentDirection == Direction::DOWN) {
        verticalMoveCounter++;
        if (verticalMoveCounter < 2) {
            return false;
        }
        verticalMoveCounter = 0;
    }
    return true;
}

Point Player::getNextPosition() const {
    Point next = position;
    if (isMoving) {
        next = next.getNextPosition(currentDirection);
    }
    return next;
}

void Player::move() {
    position = getNextPosition();
}

// Inventory management
char Player::getHeldItem() const { return heldItem; }
void Player::pickupItem(char item) { heldItem = item; }

char Player::disposeItem() {
    char item = heldItem;
    heldItem = 0;
    return item;
}

bool Player::hasItem() const { return heldItem != 0; }
bool Player::hasKey() const { return heldItem == GameConfig::CHAR_KEY; }
bool Player::hasTorch() const { return heldItem == GameConfig::CHAR_TORCH; }
bool Player::hasBomb() const { return heldItem == 'B'; }

// Display getters
char Player::getSymbol() const { return symbol; }
Color Player::getColor() const { return color; }

void Player::draw() const {
    if (silentMode) return;
    gotoxy(position.getX(), position.getY());
    ::setColor(color);
    std::cout << symbol;
    resetColor();
}

void Player::erase() const {
    if (silentMode) return;
    gotoxy(position.getX(), position.getY());
    std::cout << ' ';
}

// Spring momentum - set launch parameters
void Player::setSpringMomentum(Direction dir, int speed, int duration) {
    springDirection = dir;
    springSpeed = speed;
    springDuration = duration;
    hasSpringMomentum = true;
}

bool Player::hasActiveSpringMomentum() const {
    return hasSpringMomentum && springDuration > 0;
}

Direction Player::getSpringDirection() const { return springDirection; }
int Player::getSpringSpeed() const { return springSpeed; }

void Player::updateSpringMomentum() {
    if (hasSpringMomentum && springDuration > 0) {
        springDuration--;
        if (springDuration <= 0) {
            clearSpringMomentum();
        }
    }
}

void Player::clearSpringMomentum() {
    springDirection = Direction::STAY;
    springSpeed = 0;
    springDuration = 0;
    hasSpringMomentum = false;
}

// Get pushing force (spring momentum adds extra force)
int Player::getForce() const {
    if (hasActiveSpringMomentum()) {
        return springSpeed;  // spring launch gives extra push
    }
    return 1;
}

// Cooperative pushing - track recent movement for grace period
Direction Player::getLastMoveDirection() const { return lastMoveDirection; }
int Player::getLastMoveAge() const { return lastMoveAge; }

void Player::updateLastMoveAge() {
    if (lastMoveAge < 999) {
        lastMoveAge++;
    }
}
