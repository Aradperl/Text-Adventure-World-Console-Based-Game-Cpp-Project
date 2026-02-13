// Point.cpp - 2D coordinate with direction support

#include "Point.h"
#include "GameConfig.h"

// Constructors
Point::Point() : x(0), y(0) {}
Point::Point(int x, int y) : x(x), y(y) {}

// Getters
int Point::getX() const { return x; }
int Point::getY() const { return y; }

// Setters
void Point::setX(int newX) { x = newX; }
void Point::setY(int newY) { y = newY; }
void Point::set(int newX, int newY) { x = newX; y = newY; }

// Get next position in direction
Point Point::getNextPosition(Direction dir) const {
    Point next = *this;
    switch (dir) {
        case Direction::UP:    next.y--; break;
        case Direction::DOWN:  next.y++; break;
        case Direction::LEFT:  next.x--; break;
        case Direction::RIGHT: next.x++; break;
        case Direction::STAY:  break;
    }
    return next;
}

// Move in direction
void Point::move(Direction dir) {
    *this = getNextPosition(dir);
}

// Check bounds
bool Point::isInBounds() const {
    return x >= 0 && x < GameConfig::GAME_WIDTH &&
           y >= 0 && y < GameConfig::GAME_HEIGHT;
}

// Comparison operators
bool Point::operator==(const Point& other) const {
    return x == other.x && y == other.y;
}

bool Point::operator!=(const Point& other) const {
    return !(*this == other);
}

bool Point::operator<(const Point& other) const {
    if (x != other.x) {
        return x < other.x;
    }
    return y < other.y;
}
