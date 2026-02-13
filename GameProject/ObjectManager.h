// ObjectManager.h - Manages all game objects

#pragma once

#include "Wall.h"
#include "Key.h"
#include "Coin.h"
#include "Door.h"
#include "LightSwitch.h"
#include "Switch.h"
#include "Torch.h"
#include "Gate.h"
#include "Riddle.h"
#include "Bomb.h"
#include "Obstacle.h"
#include "Spring.h"
#include "GameBoard.h"
#include "LightingSystem.h"
#include <vector>
#include <memory>
#include <string>

class ObjectManager {
public:
    // Dropped key
    struct DroppedKeyItem {
        Point position;
        bool collected;
        DroppedKeyItem(int x, int y);
    };

private:
    // All game objects
    std::vector<std::unique_ptr<Wall>> walls;
    std::vector<std::unique_ptr<Key>> keys;
    std::vector<std::unique_ptr<Coin>> coins;
    std::vector<std::unique_ptr<Door>> doors;
    std::vector<std::unique_ptr<LightSwitch>> lightSwitches;
    std::vector<std::unique_ptr<Switch>> switches;
    std::vector<std::unique_ptr<Torch>> torches;
    std::vector<std::unique_ptr<Riddle>> riddles;
    std::vector<Gate> gates;
    std::vector<DroppedKeyItem> droppedKeys;
    std::vector<DroppedTorch> droppedTorches;
    
    // New objects
    std::vector<std::unique_ptr<Bomb>> bombs;
    std::vector<std::unique_ptr<Obstacle>> obstacles;
    std::vector<std::unique_ptr<Spring>> springs;
    
    // Temporary storage for obstacle/spring cells during parsing
    std::vector<Point> pendingObstacleCells;
    std::vector<Point> pendingSpringCells;

    // Board reference (not owned)
    GameBoard* board;

public:
    ObjectManager();

    // Setup
    void setBoard(GameBoard* b);

    // Add objects
    void addWall(int x, int y);
    void addHorizontalWall(int x, int y, int length);
    void addVerticalWall(int x, int y, int length);
    void addKey(int x, int y);
    void addCoin(int x, int y);
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
    void addObstacleCell(int x, int y);
    void addSpringCell(int x, int y);
    void finalizeObstacles();  // Group adjacent cells into obstacles
    void finalizeSprings();    // Finalize spring orientations

    // Drop items
    void dropKey(const Point& pos);
    void dropTorch(const Point& pos, int radius);

    // Get objects at position
    Key* getKeyAt(const Point& p);
    Coin* getCoinAt(const Point& p);
    Door* getDoorAt(const Point& p);
    LightSwitch* getLightSwitchAt(const Point& p);
    Switch* getSwitchAt(const Point& p);
    Torch* getTorchAt(const Point& p);
    Gate* getGateAt(const Point& p);
    Gate* getGateByDoorNum(char doorNum);
    DroppedKeyItem* getDroppedKeyAt(const Point& p);
    DroppedTorch* getDroppedTorchAt(const Point& p);
    Riddle* getRiddleAt(const Point& p);
    Bomb* getBombAt(const Point& p);
    Obstacle* getObstacleAt(const Point& p);
    Spring* getSpringAt(const Point& p);
    
    // Get all objects (for updates)
    std::vector<std::unique_ptr<Bomb>>& getBombs();
    std::vector<std::unique_ptr<Obstacle>>& getObstacles();
    std::vector<std::unique_ptr<Spring>>& getSprings();

    // Switch-gate logic
    bool areAllSwitchesOnForGate(char doorNum) const;

    // Check for keys in room
    bool hasKeysInRoom() const;

    // Update board
    void updateBoard();

    // Reset
    void reset();

    // Get references for lighting
    const std::vector<std::unique_ptr<LightSwitch>>* getLightSwitches() const;
    const std::vector<std::unique_ptr<Torch>>* getTorches() const;
    const std::vector<DroppedTorch>* getDroppedTorches() const;

    // Riddle management
    std::vector<std::unique_ptr<Riddle>>& getRiddles() { return riddles; }
    void clearRiddles() { riddles.clear(); }

    // Destruction helper (used by bomb shrapnel)
    bool destroyAt(const Point& p);
};