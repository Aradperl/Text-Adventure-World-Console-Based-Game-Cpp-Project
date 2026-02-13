// Obstacle.cpp - Multi-block pushable obstacles
// Supports cooperative pushing with force calculation
// AI usage: Force-based pushing (AI-assisted), flood-fill grouping (AI-assisted)

#include "Obstacle.h"
#include "GameBoard.h"
#include "GameConfig.h"
#include "console.h"
#include <iostream>
#include <algorithm>

Obstacle::Obstacle() : GameObject(0, 0, '*', Color::Gray), size(0) {}

Obstacle::Obstacle(int x, int y) : GameObject(x, y, '*', Color::Gray), size(0) {
    addBlock(x, y);
}

void Obstacle::addBlock(int x, int y) {
    blocks.push_back(Point(x, y));
    size = (int)blocks.size();
    
    if (blocks.size() == 1) {
        position = blocks[0];
    }
}

void Obstacle::addBlock(const Point& p) {
    addBlock(p.getX(), p.getY());
}

bool Obstacle::containsPoint(const Point& p) const {
    return containsPoint(p.getX(), p.getY());
}

bool Obstacle::containsPoint(int x, int y) const {
    for (const auto& block : blocks) {
        if (block.getX() == x && block.getY() == y) {
            return true;
        }
    }
    return false;
}

// AI-assisted: Check if obstacle can be pushed with given force
// Requires combined force >= obstacle size, and clear destination cells
bool Obstacle::canPush(Direction dir, GameBoard* board, int force) const {
    if (!board || force < size) return false;
    
    int dx = 0, dy = 0;
    if (dir == Direction::UP) dy = -1;
    else if (dir == Direction::DOWN) dy = 1;
    else if (dir == Direction::LEFT) dx = -1;
    else if (dir == Direction::RIGHT) dx = 1;
    else return false;
    
    // Check each block can move to its new position
    for (const auto& block : blocks) {
        int newX = block.getX() + dx;
        int newY = block.getY() + dy;
        
        char cell = board->getCell(newX, newY);
        
        if (cell == 'W') return false;  // walls block
        if (containsPoint(newX, newY)) continue;  // own block is fine
        
        if (cell != ' ' && cell != GameConfig::CHAR_EMPTY) {
            return false;
        }
    }
    
    return true;
}

// Move all blocks one cell in direction
bool Obstacle::push(Direction dir, GameBoard* board, int force) {
    if (!canPush(dir, board, force)) return false;
    
    int dx = 0, dy = 0;
    if (dir == Direction::UP) dy = -1;
    else if (dir == Direction::DOWN) dy = 1;
    else if (dir == Direction::LEFT) dx = -1;
    else if (dir == Direction::RIGHT) dx = 1;
    else return false;
    
    clearFromBoard(board);
    
    for (auto& block : blocks) {
        block.setX(block.getX() + dx);
        block.setY(block.getY() + dy);
    }
    
    if (!blocks.empty()) {
        position = blocks[0];
    }
    
    updateBoard(board);
    
    return true;
}

void Obstacle::updateBoard(GameBoard* board) const {
    if (!board) return;
    
    for (const auto& block : blocks) {
        board->setCell(block.getX(), block.getY(), '*');
    }
}

void Obstacle::clearFromBoard(GameBoard* board) const {
    if (!board) return;
    
    for (const auto& block : blocks) {
        board->setCell(block.getX(), block.getY(), ' ');
    }
}

void Obstacle::draw() const {
    ::setColor(Color::Gray);
    for (const auto& block : blocks) {
        gotoxy(block.getX(), block.getY());
        std::cout << '*';
    }
    resetColor();
}

void Obstacle::getBounds(int& minX, int& minY, int& maxX, int& maxY) const {
    if (blocks.empty()) {
        minX = minY = maxX = maxY = 0;
        return;
    }
    
    minX = maxX = blocks[0].getX();
    minY = maxY = blocks[0].getY();
    
    for (const auto& block : blocks) {
        if (block.getX() < minX) minX = block.getX();
        if (block.getX() > maxX) maxX = block.getX();
        if (block.getY() < minY) minY = block.getY();
        if (block.getY() > maxY) maxY = block.getY();
    }
}

// Save starting positions for level reset
void Obstacle::saveOriginalPositions() {
    originalBlocks = blocks;
}

void Obstacle::reset() {
    if (!originalBlocks.empty()) {
        blocks = originalBlocks;
        if (!blocks.empty()) {
            position = blocks[0];
        }
        size = (int)blocks.size();
        active = true;
    }
}

// --- ObstacleManager ---

// AI-assisted: Recursive flood fill to connect adjacent obstacle cells
void ObstacleManager::floodFill(int x, int y, std::vector<std::vector<bool>>& visited,
                                 const std::vector<std::string>& grid, Obstacle& obstacle) {
    if (y < 0 || y >= (int)grid.size()) return;
    if (x < 0 || x >= (int)grid[y].size()) return;
    if (visited[y][x]) return;
    if (grid[y][x] != '*') return;
    
    visited[y][x] = true;
    obstacle.addBlock(x, y);
    
    // 4-directional neighbors
    floodFill(x + 1, y, visited, grid, obstacle);
    floodFill(x - 1, y, visited, grid, obstacle);
    floodFill(x, y + 1, visited, grid, obstacle);
    floodFill(x, y - 1, visited, grid, obstacle);
}

// Build obstacles by grouping connected * characters
void ObstacleManager::buildFromGrid(const std::vector<std::string>& grid) {
    obstacles.clear();
    
    if (grid.empty()) return;
    
    int height = (int)grid.size();
    int width = 0;
    for (const auto& row : grid) {
        if ((int)row.size() > width) width = (int)row.size();
    }
    
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < (int)grid[y].size(); x++) {
            if (grid[y][x] == '*' && !visited[y][x]) {
                Obstacle obs;
                floodFill(x, y, visited, grid, obs);
                if (obs.getSize() > 0) {
                    obstacles.push_back(obs);
                }
            }
        }
    }
}

Obstacle* ObstacleManager::getObstacleAt(const Point& p) {
    return getObstacleAt(p.getX(), p.getY());
}

Obstacle* ObstacleManager::getObstacleAt(int x, int y) {
    for (auto& obs : obstacles) {
        if (obs.containsPoint(x, y)) {
            return &obs;
        }
    }
    return nullptr;
}

bool ObstacleManager::hasObstacleAt(const Point& p) const {
    return hasObstacleAt(p.getX(), p.getY());
}

bool ObstacleManager::hasObstacleAt(int x, int y) const {
    for (const auto& obs : obstacles) {
        if (obs.containsPoint(x, y)) {
            return true;
        }
    }
    return false;
}

bool ObstacleManager::tryPush(const Point& pos, Direction dir, GameBoard* board, int force) {
    Obstacle* obs = getObstacleAt(pos);
    if (!obs) return false;
    
    return obs->push(dir, board, force);
}

void ObstacleManager::updateBoard(GameBoard* board) const {
    for (const auto& obs : obstacles) {
        obs.updateBoard(board);
    }
}

void ObstacleManager::drawAll() const {
    for (const auto& obs : obstacles) {
        obs.draw();
    }
}
