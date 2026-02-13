// InteractionHandler.h - Player-object interactions

#pragma once

#include "Player.h"
#include "ObjectManager.h"
#include "GameBoard.h"
#include "LightingSystem.h"
#include "StatusBar.h"
#include "MessageDisplay.h"
#include "Riddle.h"

class Game;
class GameRecorder;

// Result from interaction
struct InteractionResult {
    bool needsRedraw;
    bool needsFullRedraw;
    bool gateOpened;
    bool gameOver;

    InteractionResult() : needsRedraw(false), needsFullRedraw(false), gateOpened(false), gameOver(false) {}
};

class InteractionHandler {
private:
    ObjectManager* objects;
    GameBoard* board;
    LightingSystem* lighting;
    StatusBar* statusBar;
    MessageDisplay* messageDisplay;
    Game* game;
    GameRecorder* recorder;

    // Track riddle to prevent re-trigger
    Riddle* lastTriggeredRiddle;
    Point lastRiddlePos;
    
    // Store last riddle answer for recording
    char lastRiddleAnswer;

public:
    InteractionHandler();

    // Setup
    void setObjectManager(ObjectManager* obj);
    void setBoard(GameBoard* b);
    void setLightingSystem(LightingSystem* light);
    void setStatusBar(StatusBar* status);
    void setMessageDisplay(MessageDisplay* msg);
    void setGame(Game* g);
    void setRecorder(GameRecorder* rec);
    
    // Get last riddle answer (for recording)
    char getLastRiddleAnswer() const { return lastRiddleAnswer; }

    // Handle interactions at position
    InteractionResult handleInteractions(Player& player);

    // Gate handling (returns true if opened)
    bool tryOpenGate(Player& player, const Point& nextPos);

    // Item drop
    void dropPlayerItem(Player& player);

    // Open/close gate
    void openGate(Gate& gate);
    void closeGate(Gate& gate);

    // Riddle (blocking - freezes game, returns true if correct)
    bool handleRiddle(Player& player, Riddle& riddle, int playerNum);

private:
    void displayRiddleUI(const Riddle& riddle);
    void clearRiddleUI();
    char waitForRiddleAnswer(int playerNum);
};