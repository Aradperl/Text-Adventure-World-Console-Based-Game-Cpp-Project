// GameBoard.h - Board management and cell access

#pragma once

#include "GameConfig.h"
#include "Point.h"

class GameBoard {
private:
    char cells[GameConfig::GAME_HEIGHT][GameConfig::GAME_WIDTH];

public:
    GameBoard();

    // Board operations
    void clear();

    // Get/set cells
    char getCell(int x, int y) const;
    char getCell(const Point& p) const;
    void setCell(int x, int y, char c);
    void setCell(const Point& p, char c);

    // Check cell types
    bool isWall(const Point& p) const;
    bool isDoor(const Point& p) const;
    bool isGateBar(const Point& p) const;
    bool isBlocked(const Point& p) const;

    // Direct board access (used by Gate)
    char (*getRawBoard())[GameConfig::GAME_WIDTH];

    // Drawing
    void drawCell(int x, int y, bool isVisible = true) const;
    void draw(bool (*isVisibleFunc)(int, int) = nullptr) const;
};
