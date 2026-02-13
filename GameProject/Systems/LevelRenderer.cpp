// LevelRenderer.cpp - Renders game board with visibility and lighting

#include "LevelRenderer.h"
#include "GameConfig.h"
#include "console.h"
#include <algorithm>
#include <iostream>

// Constructor
LevelRenderer::LevelRenderer()
    : board(nullptr), lighting(nullptr), player1(nullptr), player2(nullptr), silentMode(false) {
}

// Setup
void LevelRenderer::setBoard(GameBoard* b) {
    board = b;
}

void LevelRenderer::setLightingSystem(LightingSystem* light) {
    lighting = light;
}

void LevelRenderer::setPlayers(Player* p1, Player* p2) {
    player1 = p1;
    player2 = p2;
}

void LevelRenderer::setSilentMode(bool silent) {
    silentMode = silent;
}

// Draw single cell
void LevelRenderer::drawCell(int x, int y) const {
    if (silentMode || !board) return;

    bool visible = lighting ? lighting->isCellVisible(x, y) : true;
    board->drawCell(x, y, visible);
}

// Draw entire board
void LevelRenderer::drawBoard() const {
    if (silentMode || !board) return;

    for (int y = 0; y < GameConfig::GAME_HEIGHT; y++) {
        gotoxy(0, y);
        for (int x = 0; x < GameConfig::GAME_WIDTH; x++) {
            bool visible = lighting ? lighting->isCellVisible(x, y) : true;

            if (!visible) {
                ::setColor(Color::Black);
                std::cout << ' ';
                continue;
            }

            char c = board->getCell(x, y);
            switch (c) {
            case GameConfig::CHAR_WALL:
                ::setColor(Color::Gray);
                break;
            case GameConfig::CHAR_KEY:
                ::setColor(Color::Yellow);
                break;
            case GameConfig::CHAR_COIN:
                ::setColor(Color::Yellow);
                break;
            case GameConfig::CHAR_TORCH:
                ::setColor(Color::LightRed);
                break;
            case '/':
                ::setColor(Color::LightGreen);  // Switch ON
                break;
            case '\\':
                ::setColor(Color::Gray);  // Switch OFF
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

// Draw players
void LevelRenderer::drawPlayers() const {
    if (silentMode) return;
    if (player1) player1->draw();
    if (player2) player2->draw();
}

// Redraw torch area
void LevelRenderer::redrawTorchArea(int centerX, int centerY, int radius) const {
    if (silentMode || !board) return;

    int startX = (std::max)(0, centerX - radius);
    int endX = (std::min)(GameConfig::GAME_WIDTH - 1, centerX + radius);
    int startY = (std::max)(0, centerY - radius);
    int endY = (std::min)(GameConfig::GAME_HEIGHT - 1, centerY + radius);

    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            drawCell(x, y);
        }
    }
}