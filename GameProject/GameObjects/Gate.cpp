// Gate.cpp - Rectangular gate barrier opened by keys or switches
// AI usage: Bar clearing with corner preservation (AI-assisted)

#include "Gate.h"
#include "console.h"
#include <iostream>

// Constructor with position, size, and door number
Gate::Gate(int gx, int gy, int w, int h, char num)
    : x(gx), y(gy), width(w), height(h), isOpen(false), doorNum(num) {}

// Getters
int Gate::getX() const { return x; }
int Gate::getY() const { return y; }
int Gate::getWidth() const { return width; }
int Gate::getHeight() const { return height; }
bool Gate::getIsOpen() const { return isOpen; }
char Gate::getDoorNum() const { return doorNum; }

// State management
void Gate::open() { isOpen = true; }
void Gate::close() { isOpen = false; }
void Gate::reset() { isOpen = false; }

// Check if point is on gate bar
bool Gate::isOnGateBar(const Point& p) const {
    if (isOpen) return false;
    
    int px = p.getX();
    int py = p.getY();
    
    // Top and bottom bars
    if ((py == y || py == y + height - 1) &&
        px >= x && px < x + width) {
        return true;
    }
    // Left and right bars
    if ((px == x || px == x + width - 1) &&
        py >= y && py < y + height) {
        return true;
    }
    return false;
}

// Check if position is a corner
bool Gate::isAtCorner(int px, int py) const {
    int tx1 = x;
    int tx2 = x + width - 1;
    int ty1 = y;
    int ty2 = y + height - 1;
    
    return (px == tx1 && py == ty1) ||
           (px == tx2 && py == ty1) ||
           (px == tx1 && py == ty2) ||
           (px == tx2 && py == ty2);
}

// Update board with gate representation
void Gate::updateBoard(char board[][GameConfig::GAME_WIDTH]) const {
    if (!isOpen) {
        // Closed gate - draw bars
        for (int i = 0; i < width; i++) {
            int tx = x + i;
            int ty1 = y;
            int ty2 = y + height - 1;
            if (tx >= 0 && tx < GameConfig::GAME_WIDTH) {
                if (ty1 >= 0 && ty1 < GameConfig::GAME_HEIGHT) board[ty1][tx] = '-';
                if (ty2 >= 0 && ty2 < GameConfig::GAME_HEIGHT) board[ty2][tx] = '-';
            }
        }
        for (int i = 0; i < height; i++) {
            int ty = y + i;
            int tx1 = x;
            int tx2 = x + width - 1;
            if (ty >= 0 && ty < GameConfig::GAME_HEIGHT) {
                if (tx1 >= 0 && tx1 < GameConfig::GAME_WIDTH) board[ty][tx1] = '|';
                if (tx2 >= 0 && tx2 < GameConfig::GAME_WIDTH) board[ty][tx2] = '|';
            }
        }
    } else {
        // Open gate - just corners
        int tx1 = x;
        int tx2 = x + width - 1;
        int ty1 = y;
        int ty2 = y + height - 1;
        if (tx1 >= 0 && tx1 < GameConfig::GAME_WIDTH && ty1 >= 0 && ty1 < GameConfig::GAME_HEIGHT)
            board[ty1][tx1] = '+';
        if (tx2 >= 0 && tx2 < GameConfig::GAME_WIDTH && ty1 >= 0 && ty1 < GameConfig::GAME_HEIGHT)
            board[ty1][tx2] = '+';
        if (tx1 >= 0 && tx1 < GameConfig::GAME_WIDTH && ty2 >= 0 && ty2 < GameConfig::GAME_HEIGHT)
            board[ty2][tx1] = '+';
        if (tx2 >= 0 && tx2 < GameConfig::GAME_WIDTH && ty2 >= 0 && ty2 < GameConfig::GAME_HEIGHT)
            board[ty2][tx2] = '+';
    }
}

// AI-assisted: Clear gate bars while preserving corner markers
void Gate::clearBarsFromBoard(char board[][GameConfig::GAME_WIDTH]) const {
    // Clear horizontal bars (except corners)
    for (int i = 1; i < width - 1; i++) {
        int tx = x + i;
        if (tx >= 0 && tx < GameConfig::GAME_WIDTH) {
            if (y >= 0 && y < GameConfig::GAME_HEIGHT) 
                board[y][tx] = GameConfig::CHAR_EMPTY;
            int by = y + height - 1;
            if (by >= 0 && by < GameConfig::GAME_HEIGHT) 
                board[by][tx] = GameConfig::CHAR_EMPTY;
        }
    }
    // Clear vertical bars (except corners)
    for (int i = 1; i < height - 1; i++) {
        int ty = y + i;
        if (ty >= 0 && ty < GameConfig::GAME_HEIGHT) {
            if (x >= 0 && x < GameConfig::GAME_WIDTH) 
                board[ty][x] = GameConfig::CHAR_EMPTY;
            int rx = x + width - 1;
            if (rx >= 0 && rx < GameConfig::GAME_WIDTH) 
                board[ty][rx] = GameConfig::CHAR_EMPTY;
        }
    }
    // Set corners to '+'
    int tx1 = x;
    int tx2 = x + width - 1;
    int ty1 = y;
    int ty2 = y + height - 1;
    if (tx1 >= 0 && tx1 < GameConfig::GAME_WIDTH && ty1 >= 0 && ty1 < GameConfig::GAME_HEIGHT)
        board[ty1][tx1] = '+';
    if (tx2 >= 0 && tx2 < GameConfig::GAME_WIDTH && ty1 >= 0 && ty1 < GameConfig::GAME_HEIGHT)
        board[ty1][tx2] = '+';
    if (tx1 >= 0 && tx1 < GameConfig::GAME_WIDTH && ty2 >= 0 && ty2 < GameConfig::GAME_HEIGHT)
        board[ty2][tx1] = '+';
    if (tx2 >= 0 && tx2 < GameConfig::GAME_WIDTH && ty2 >= 0 && ty2 < GameConfig::GAME_HEIGHT)
        board[ty2][tx2] = '+';
}

// Draw closed gate with bars
void Gate::drawClosed() const {
    if (isOpen) return;
    ::setColor(Color::Red);
    // Horizontal bars
    for (int i = 0; i < width; i++) {
        int tx = x + i;
        if (tx >= 0 && tx < GameConfig::GAME_WIDTH) {
            if (y >= 0 && y < GameConfig::GAME_HEIGHT) {
                gotoxy(tx, y);
                std::cout << '-';
            }
            int by = y + height - 1;
            if (by >= 0 && by < GameConfig::GAME_HEIGHT) {
                gotoxy(tx, by);
                std::cout << '-';
            }
        }
    }
    // Vertical bars
    for (int i = 0; i < height; i++) {
        int ty = y + i;
        if (ty >= 0 && ty < GameConfig::GAME_HEIGHT) {
            if (x >= 0 && x < GameConfig::GAME_WIDTH) {
                gotoxy(x, ty);
                std::cout << '|';
            }
            int rx = x + width - 1;
            if (rx >= 0 && rx < GameConfig::GAME_WIDTH) {
                gotoxy(rx, ty);
                std::cout << '|';
            }
        }
    }
    resetColor();
}

// Draw open gate (just corners)
void Gate::drawOpen() const {
    ::setColor(Color::Green);
    int tx1 = x;
    int tx2 = x + width - 1;
    int ty1 = y;
    int ty2 = y + height - 1;
    
    if (tx1 >= 0 && tx1 < GameConfig::GAME_WIDTH && ty1 >= 0 && ty1 < GameConfig::GAME_HEIGHT) {
        gotoxy(tx1, ty1); std::cout << '+';
    }
    if (tx2 >= 0 && tx2 < GameConfig::GAME_WIDTH && ty1 >= 0 && ty1 < GameConfig::GAME_HEIGHT) {
        gotoxy(tx2, ty1); std::cout << '+';
    }
    if (tx1 >= 0 && tx1 < GameConfig::GAME_WIDTH && ty2 >= 0 && ty2 < GameConfig::GAME_HEIGHT) {
        gotoxy(tx1, ty2); std::cout << '+';
    }
    if (tx2 >= 0 && tx2 < GameConfig::GAME_WIDTH && ty2 >= 0 && ty2 < GameConfig::GAME_HEIGHT) {
        gotoxy(tx2, ty2); std::cout << '+';
    }
    resetColor();
}

// Draw gate in current state
void Gate::draw() const {
    if (isOpen) {
        drawOpen();
    } else {
        drawClosed();
    }
}
