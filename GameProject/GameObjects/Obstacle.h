// Obstacle.h - Pushable obstacle block

#pragma once

#include "GameObject.h"
#include "Point.h"
#include "Direction.h"
#include <vector>

class GameBoard;

class Obstacle : public GameObject {
private:
    std::vector<Point> blocks;          // Current positions of all blocks
    std::vector<Point> originalBlocks;  // Original positions (for reset)
    int size;                           // Number of blocks = force required to push
    
public:
    Obstacle();
    Obstacle(int x, int y);  // Create with initial block
    
    // Add a block to this obstacle
    void addBlock(int x, int y);
    void addBlock(const Point& p);
    
    // Get all blocks
    const std::vector<Point>& getBlocks() const { return blocks; }
    
    // Get size (force required to push)
    int getSize() const { return size; }
    
    // Check if a point is part of this obstacle
    bool containsPoint(const Point& p) const;
    bool containsPoint(int x, int y) const;
    
    // Check if obstacle can be pushed in direction
    bool canPush(Direction dir, GameBoard* board, int force) const;
    
    // Push the obstacle in direction (returns true if successful)
    bool push(Direction dir, GameBoard* board, int force);
    
    // Update board with obstacle positions
    void updateBoard(GameBoard* board) const;
    
    // Clear obstacle from board
    void clearFromBoard(GameBoard* board) const;
    
    // Draw
    void draw() const override;
    
    // Get bounding box
    void getBounds(int& minX, int& minY, int& maxX, int& maxY) const;
    
    // Save original positions (call after obstacle is fully built)
    void saveOriginalPositions();
    
    // Reset to original positions
    void reset();
};

// Obstacle Manager - handles grouping adjacent '*' into obstacles
class ObstacleManager {
private:
    std::vector<Obstacle> obstacles;
    
    // Flood fill to find connected blocks
    void floodFill(int x, int y, std::vector<std::vector<bool>>& visited,
                   const std::vector<std::string>& grid, Obstacle& obstacle);
    
public:
    // Build obstacles from grid of characters
    void buildFromGrid(const std::vector<std::string>& grid);
    
    // Get obstacle at position
    Obstacle* getObstacleAt(const Point& p);
    Obstacle* getObstacleAt(int x, int y);
    
    // Check if position has obstacle
    bool hasObstacleAt(const Point& p) const;
    bool hasObstacleAt(int x, int y) const;
    
    // Try to push obstacle at position
    bool tryPush(const Point& pos, Direction dir, GameBoard* board, int force);
    
    // Update all obstacles on board
    void updateBoard(GameBoard* board) const;
    
    // Draw all obstacles
    void drawAll() const;
    
    // Get all obstacles
    std::vector<Obstacle>& getObstacles() { return obstacles; }
    const std::vector<Obstacle>& getObstacles() const { return obstacles; }
    
    // Clear
    void clear() { obstacles.clear(); }
};
