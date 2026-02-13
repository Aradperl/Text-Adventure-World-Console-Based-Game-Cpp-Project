// StatusBar.h - Status bar at bottom of screen

#pragma once

#include "Player.h"

class StatusBar {
private:
    int levelNumber;
    bool isDarkRoom;
    bool isRoomLit;
    const Player* player1;
    const Player* player2;
    int bombCountdown;  // 0 means no active planted bomb
    static bool silentMode;

public:
    StatusBar();

    // Setup
    void setLevelNumber(int num);
    void setPlayers(const Player* p1, const Player* p2);
    void setDarkRoomStatus(bool dark, bool lit);
    void setBombCountdown(int ticks);  // ticks 5..1 shown, 0 hides
    static void setSilentMode(bool silent);

    // Draw
    void draw() const;
};
