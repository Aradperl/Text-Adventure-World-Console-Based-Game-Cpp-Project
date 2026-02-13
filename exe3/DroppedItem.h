// DroppedItem.h - Base class and derived classes for dropped items

#pragma once

#include "Point.h"
#include "GameConfig.h"

// Base class for dropped items in the game
class DroppedItem {
protected:
    Point position;
    bool collected;

public:
    DroppedItem(int x, int y);
    virtual ~DroppedItem() = default;

    // Getters
    const Point& getPosition() const;
    int getX() const;
    int getY() const;
    bool isCollected() const;

    // Actions
    void collect();
    
    // Check if at position
    bool isAt(const Point& p) const;

    // Get symbol (to be overridden)
    virtual char getSymbol() const = 0;
};

// Dropped key item
class DroppedKeyClass : public DroppedItem {
public:
    DroppedKeyClass(int x, int y);
    char getSymbol() const override;
};
