// Point.h - 2D position class

#pragma once

#include "Direction.h"

class Point {
private:
    int x;
    int y;
    
public:
    Point();
    Point(int x, int y);
    
    // Getters
    int getX() const;
    int getY() const;
    
    // Setters
    void setX(int newX);
    void setY(int newY);
    void set(int newX, int newY);
    
    // Movement
    Point getNextPosition(Direction dir) const;
    void move(Direction dir);
    bool isInBounds() const;
    
    // Comparison
    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;
    bool operator<(const Point& other) const;
};
