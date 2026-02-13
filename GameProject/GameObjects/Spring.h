// Spring.h - Spring mechanism for launching players

#pragma once

#include "GameObject.h"
#include "Point.h"
#include "Direction.h"
#include <vector>

class GameBoard;
class Player;

class Spring : public GameObject {
private:
    std::vector<Point> blocks;      // All spring characters (original positions)
    Direction orientation;          // Row (Left/Right) or Column (Up/Down)
    Direction releaseDir;           // Direction spring releases toward
    Point wallEnd;                  // The end adjacent to wall
    int originalLength;             // Original number of spring chars
    int currentCompression;         // How many chars are currently compressed
    bool isCompressed;              // Is spring currently compressed
    bool hasBeenUsed;               // Has spring been released at least once
    Player* compressingPlayer;      // Player currently on spring
    Point playerPosition;           // Current position of compressing player

public:
    Spring();

    // Setup
    void addBlock(int x, int y);
    void addBlock(const Point& p);
    void setOrientation(Direction dir);
    void setReleaseDirection(Direction dir);
    void setWallEnd(const Point& p);
    void finalize();  // Call after adding all blocks

    // Getters
    const std::vector<Point>& getBlocks() const { return blocks; }
    int getLength() const { return originalLength; }
    int getCompression() const { return currentCompression; }
    bool getIsCompressed() const { return isCompressed; }
    bool getHasBeenUsed() const { return hasBeenUsed; }
    Direction getReleaseDirection() const { return releaseDir; }
    Direction getOrientation() const { return orientation; }
    Player* getCompressingPlayer() const { return compressingPlayer; }
    const Point& getWallEnd() const { return wallEnd; }

    // Check if point is on spring (original positions)
    bool containsPoint(const Point& p) const;
    bool containsPoint(int x, int y) const;

    // Check if point is on an uncompressed spring block
    bool isBlockVisible(const Point& p) const;

    // Get the entry point (opposite of wall end)
    Point getEntryPoint() const;

    // Player interaction
    void onPlayerEnter(Player* player, const Point& pos);
    void onPlayerMove(Player* player, const Point& newPos, Direction moveDir);
    void onPlayerStay(Player* player);
    void onPlayerExit(Player* player);

    // Check if player should trigger release
    bool shouldRelease(Direction playerDir) const;

    // Release spring - returns speed and duration
    struct ReleaseInfo {
        int speed;
        int duration;
        Direction direction;
        bool released;
    };
    ReleaseInfo release();

    // Reset compression
    void resetCompression();

    // Update board - only draw uncompressed blocks
    void updateBoard(GameBoard* board) const;

    // Draw - only draw uncompressed blocks
    void draw() const override;
};

// Spring effect on player
struct SpringEffect {
    bool active;
    Direction direction;
    int speed;
    int remainingCycles;
    int force;  // Force for pushing obstacles

    SpringEffect() : active(false), direction(Direction::STAY),
        speed(0), remainingCycles(0), force(1) {
    }

    void apply(Direction dir, int spd) {
        active = true;
        direction = dir;
        speed = spd;
        remainingCycles = spd * spd;  // Duration = speed^2
        force = spd;
    }

    void update() {
        if (active && remainingCycles > 0) {
            remainingCycles--;
            if (remainingCycles <= 0) {
                active = false;
                speed = 0;
                force = 1;
            }
        }
    }

    void clear() {
        active = false;
        direction = Direction::STAY;
        speed = 0;
        remainingCycles = 0;
        force = 1;
    }
};

// Spring Manager
class SpringManager {
private:
    std::vector<Spring> springs;

    // Detect spring orientation and wall end
    void analyzeSpring(Spring& spring, GameBoard* board);

public:
    // Build springs from grid
    void buildFromGrid(const std::vector<std::string>& grid, GameBoard* board);

    // Get spring at position
    Spring* getSpringAt(const Point& p);
    Spring* getSpringAt(int x, int y);

    // Check if position has spring
    bool hasSpringAt(const Point& p) const;
    bool hasSpringAt(int x, int y) const;

    // Update board
    void updateBoard(GameBoard* board) const;

    // Draw all springs
    void drawAll() const;

    // Get all springs
    std::vector<Spring>& getSprings() { return springs; }

    // Clear
    void clear() { springs.clear(); }
};
