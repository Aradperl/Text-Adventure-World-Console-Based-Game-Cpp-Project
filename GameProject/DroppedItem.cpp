// DroppedItem.cpp - Items dropped by players on the ground

#include "DroppedItem.h"

// Constructor
DroppedItem::DroppedItem(int x, int y) : position(x, y), collected(false) {}

// Get position reference
const Point& DroppedItem::getPosition() const {
    return position;
}

// Get X coordinate
int DroppedItem::getX() const {
    return position.getX();
}

// Get Y coordinate
int DroppedItem::getY() const {
    return position.getY();
}

// Check if item was collected
bool DroppedItem::isCollected() const {
    return collected;
}

// Mark item as collected
void DroppedItem::collect() {
    collected = true;
}

// Check if item is at given position
bool DroppedItem::isAt(const Point& p) const {
    return !collected && position == p;
}

// DroppedKeyClass constructor
DroppedKeyClass::DroppedKeyClass(int x, int y) : DroppedItem(x, y) {}

// Get key symbol
char DroppedKeyClass::getSymbol() const {
    return GameConfig::CHAR_KEY;
}
