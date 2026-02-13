// InputHandler.h - Keyboard input handling

#pragma once

#include "Player.h"
#include "GameConfig.h"
#include "GameRecorder.h"

// Input result
struct InputResult {
    bool escPressed;
    bool menuRequested;
    bool saveRequested;
    bool player1Dispose;
    bool player2Dispose;
    
    InputResult() : escPressed(false), menuRequested(false), saveRequested(false),
                   player1Dispose(false), player2Dispose(false) {}
};

class InputHandler {
private:
    Player* player1;
    Player* player2;
    bool isPaused;
    GameRecorder* recorder;

public:
    InputHandler();

    // Setup
    void setPlayers(Player* p1, Player* p2);
    void setRecorder(GameRecorder* rec);

    // Pause
    void setPaused(bool paused);
    bool getIsPaused() const;

    // Process input
    InputResult processInput();
    
private:
    // Handle a single key press
    void handleKey(char key, InputResult& result);
    
    // Get next step from recorder for load mode
    char getRecordedInput(int player);
};
