// GameBoard.cpp - 2D grid storing cell contents and rendering
// AI usage: Color mapping for cell types (AI-assisted)

#include "GameBoard.h"
#include "console.h"
#include <iostream>

// Initialize board with empty cells
GameBoard::GameBoard() {
    clear();
}

// Clear board to empty spaces
void GameBoard::clear() {
    for (int y = 0; y < GameConfig::GAME_HEIGHT; y++) {
        for (int x = 0; x < GameConfig::GAME_WIDTH; x++) {
            cells[y][x] = GameConfig::CHAR_EMPTY;
        }
    }
}

// Get cell at coordinates
char GameBoard::getCell(int x, int y) const {
    if (x >= 0 && x < GameConfig::GAME_WIDTH && y >= 0 && y < GameConfig::GAME_HEIGHT) {
        return cells[y][x];
    }
    return GameConfig::CHAR_WALL;  // out of bounds = wall
}

char GameBoard::getCell(const Point& p) const {
    return getCell(p.getX(), p.getY());
}

// Set cell at coordinates
void GameBoard::setCell(int x, int y, char c) {
    if (x >= 0 && x < GameConfig::GAME_WIDTH && y >= 0 && y < GameConfig::GAME_HEIGHT) {
        cells[y][x] = c;
    }
}

void GameBoard::setCell(const Point& p, char c) {
    setCell(p.getX(), p.getY(), c);
}

// Check what's at a position
bool GameBoard::isWall(const Point& p) const {
    return getCell(p) == GameConfig::CHAR_WALL;
}

bool GameBoard::isDoor(const Point& p) const {
    char c = getCell(p);
    return (c >= '1' && c <= '9');
}

bool GameBoard::isGateBar(const Point& p) const {
    char c = getCell(p);
    return (c == '-' || c == '|');
}

bool GameBoard::isBlocked(const Point& p) const {
    if (!p.isInBounds()) return true;
    return isWall(p) || isGateBar(p);
}

// Direct board access for Gate efficiency
char (*GameBoard::getRawBoard())[GameConfig::GAME_WIDTH] {
    return cells;
}

// AI-assisted: Draw cell with color based on type
void GameBoard::drawCell(int x, int y, bool isVisible) const {
    if (x < 0 || x >= GameConfig::GAME_WIDTH || y < 0 || y >= GameConfig::GAME_HEIGHT) return;

    gotoxy(x, y);

    if (!isVisible) {
        ::setColor(Color::Black);
        std::cout << ' ';
        resetColor();
        return;
    }

    char c = cells[y][x];

    // Pick color based on cell type
    switch (c) {
    case GameConfig::CHAR_WALL:
        ::setColor(Color::Gray);
        break;
    case GameConfig::CHAR_KEY:
        ::setColor(Color::Yellow);
        break;
    case GameConfig::CHAR_TORCH:
        ::setColor(Color::LightRed);
        break;
    case '/':
        ::setColor(Color::LightGreen);  // switch ON
        break;
    case '\\':
        ::setColor(Color::Gray);  // switch OFF
        break;
    case '-':
    case '|':
        ::setColor(Color::Red);  // gate bars
        break;
    case '+':
        ::setColor(Color::Green);  // open gate corners
        break;
    case '#':
        ::setColor(Color::Cyan);  // spring
        break;
    default:
        if (c >= '1' && c <= '9') {
            ::setColor(Color::Magenta);  // doors
        }
        else {
            ::setColor(Color::White);
        }
        break;
    }
    std::cout << c;
    resetColor();
}

// Draw entire board
void GameBoard::draw(bool (*isVisibleFunc)(int, int)) const {
    for (int y = 0; y < GameConfig::GAME_HEIGHT; y++) {
        gotoxy(0, y);
        for (int x = 0; x < GameConfig::GAME_WIDTH; x++) {
            bool visible = isVisibleFunc ? isVisibleFunc(x, y) : true;

            if (!visible) {
                ::setColor(Color::Black);
                std::cout << ' ';
                continue;
            }

            char c = cells[y][x];
            switch (c) {
            case GameConfig::CHAR_WALL:
                ::setColor(Color::Gray);
                break;
            case GameConfig::CHAR_KEY:
                ::setColor(Color::Yellow);
                break;
            case GameConfig::CHAR_TORCH:
                ::setColor(Color::LightRed);
                break;
            case '/':
                ::setColor(Color::LightGreen);
                break;
            case '\\':
                ::setColor(Color::Gray);  // switch OFF
                break;
            case '-':
            case '|':
                ::setColor(Color::Red);
                break;
            case '+':
                ::setColor(Color::Green);
                break;
            default:
                if (c >= '1' && c <= '9') {
                    ::setColor(Color::Magenta);
                }
                else {
                    ::setColor(Color::White);
                }
                break;
            }
            std::cout << c;
        }
        resetColor();
    }
}