// LevelRenderer.h - Rendering game level

#pragma once

#include "GameBoard.h"
#include "LightingSystem.h"
#include "Player.h"

class LevelRenderer {
private:
    GameBoard* board;
    LightingSystem* lighting;
    Player* player1;
    Player* player2;
    bool silentMode;

public:
    LevelRenderer();

    // Setup
    void setBoard(GameBoard* b);
    void setLightingSystem(LightingSystem* light);
    void setPlayers(Player* p1, Player* p2);
    void setSilentMode(bool silent);

    // Drawing
    void drawCell(int x, int y) const;
    void drawBoard() const;
    void drawPlayers() const;
    void redrawTorchArea(int centerX, int centerY, int radius) const;
};