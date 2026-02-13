// GameLevel.h - Playable game level

#pragma once

#include "BaseScreen.h"
#include "Player.h"
#include "GameBoard.h"
#include "ObjectManager.h"
#include "LightingSystem.h"
#include "CollisionSystem.h"
#include "InteractionHandler.h"
#include "InputHandler.h"
#include "LevelRenderer.h"
#include "StatusBar.h"
#include "MessageDisplay.h"
#include "PauseMenu.h"
#include "Heart.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <cstdlib>
#include <ctime>

class GameRecorder;  // Forward declaration

class GameLevel : public BaseScreen {
private:
    // Game systems
    GameBoard board;
    ObjectManager objects;
    LightingSystem lighting;
    CollisionSystem collision;
    InteractionHandler interactions;
    InputHandler input;
    LevelRenderer renderer;
    StatusBar statusBar;
    MessageDisplay messageDisplay;
    PauseMenu pauseMenu;

    // Level info
    int levelNumber;
    ScreenResult result;
    bool needsFullRedraw;
    bool silentMode;

    // Players (owned by Game class)
    Player* player1;
    Player* player2;

    // Game reference for riddle tracking
    Game* game;

    // Door crossing tracking
    bool player1CrossedDoor;
    bool player2CrossedDoor;
    int doorTargetScreen;  // Target screen from the door players used

    // Heart spawn system
    Heart heart;
    bool heartActive;
    int heartRespawnTimer;
    static const int HEART_RESPAWN_DELAY = 30;  // about 3 seconds

    struct ShrapnelParticle {
        Point pos;
        Point prevPos;
        Point origin;  // Where the bomb was placed (immune position)
        int dx;
        int dy;
        char sym;
        int stepsLeft;
        bool active;
    };
    std::vector<ShrapnelParticle> shrapnelParticles;
    static const int SHRAPNEL_MAX_STEPS = 4;  // Match explosion radius
    
    // Track which players have been hit by each explosion (key = origin point)
    std::map<Point, std::set<Player*>> explosionPlayerHits;

    // Helper methods
    void handlePlayerMovement(Player& player, Player* otherPlayer);
    bool checkBothPlayersOnDoor() const;
    void spawnHeart();
    void despawnHeart();
    void updateHeart();
    bool isValidHeartPosition(int x, int y) const;
    void updateBombs();
    void spawnShrapnel(const Point& origin);
    void updateShrapnel();
    void handleSpringInteraction(Player& player, Player* otherPlayer);
    bool tryPushObstacle(Player& player, Player* otherPlayer, Direction dir);

public:
    GameLevel(int num = 1);

    // Setup
    void setPlayers(Player* p1, Player* p2);
    void setGame(Game* g);
    void setSilentMode(bool silent);

    // Add objects
    void addWall(int x, int y);
    void addHorizontalWall(int x, int y, int length);
    void addVerticalWall(int x, int y, int length);
    void addKey(int x, int y);
    void addDoor(int x, int y, char doorNum, int targetScreen = -1);
    void addHorizontalDoor(int x, int y, int length, char doorNum, int targetScreen = -1);
    void addVerticalDoor(int x, int y, int length, char doorNum, int targetScreen = -1);
    void addGate(int x, int y, int width, int height, char doorNum);
    void addLightSwitch(int x, int y, bool startOn = false);
    void addSwitch(int x, int y, char linkedGate, bool startOn = false);
    void addTorch(int x, int y, int radius = 5);
    void addRiddle(int x, int y, const std::string& question,
        const std::vector<std::string>& options, char correctAnswer);
    
    // New objects
    void addBomb(int x, int y);
    void addCoin(int x, int y);
    void addObstacleCell(int x, int y);
    void addSpringCell(int x, int y);

    // Dark room settings
    void setDarkRoom(bool dark);
    bool getIsDarkRoom() const;
    void setTorchRadius(int radius);

    // Screen interface
    void init() override;
    void enter() override;
    void exit() override;
    void reset() override;
    void handleInput() override;
    void update() override;
    void render() override;
    ScreenResult getResult() const override;

    int getLevelNumber() const;
    int getDoorTargetScreen() const;  // Get target screen from door players used

    // Riddle management for filtering used riddles
    std::vector<std::unique_ptr<Riddle>>& getRiddles();
    void clearRiddles();
    void addRiddle(std::unique_ptr<Riddle> riddle);
};