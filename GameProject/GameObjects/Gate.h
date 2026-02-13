// Gate.h - Gate that blocks doors until opened with key

#pragma once

#include "Point.h"
#include "GameConfig.h"

class Gate {
private:
    int x, y;
    int width, height;
    bool isOpen;
    char doorNum;  // which door this gate protects

public:
    Gate(int gx, int gy, int w, int h, char num);

    // Getters
    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;
    bool getIsOpen() const;
    char getDoorNum() const;

    // State
    void open();
    void close();
    void reset();

    // Check if point is on gate bar
    bool isOnGateBar(const Point& p) const;

    // Check if point is at corner
    bool isAtCorner(int px, int py) const;

    // Board management
    void updateBoard(char board[][GameConfig::GAME_WIDTH]) const;
    void clearBarsFromBoard(char board[][GameConfig::GAME_WIDTH]) const;

    // Drawing
    void drawClosed() const;
    void drawOpen() const;
    void draw() const;
};
