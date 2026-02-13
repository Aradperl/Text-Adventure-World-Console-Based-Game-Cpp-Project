// Spring.cpp - Compressible spring mechanics for launching players
// Handles compression, release conditions, and momentum calculation
// AI usage: Block sorting for compression (AI-assisted), release conditions (AI-assisted),
//           wall detection for orientation (AI-assisted)

#include "Spring.h"
#include "GameBoard.h"
#include "GameConfig.h"
#include "Player.h"
#include "console.h"
#include <iostream>
#include <algorithm>
#include <cmath>

Spring::Spring() 
    : GameObject(0, 0, '#', Color::Cyan),
    orientation(Direction::STAY),
    releaseDir(Direction::STAY),
    originalLength(0),
    currentCompression(0),
    isCompressed(false),
    hasBeenUsed(false),
    compressingPlayer(nullptr) {
}

void Spring::addBlock(int x, int y) {
    blocks.push_back(Point(x, y));
    if (blocks.size() == 1) {
        position = blocks[0];
    }
}

void Spring::addBlock(const Point& p) {
    addBlock(p.getX(), p.getY());
}

void Spring::setOrientation(Direction dir) { orientation = dir; }
void Spring::setReleaseDirection(Direction dir) { releaseDir = dir; }
void Spring::setWallEnd(const Point& p) { wallEnd = p; }

// AI-assisted: Sort blocks from wall end to entry point for proper compression
// Wall end comes first so compression removes blocks from entry side
void Spring::finalize() {
    originalLength = (int)blocks.size();

    // Order matters: wall end comes first
    if (orientation == Direction::LEFT || orientation == Direction::RIGHT) {
        if (releaseDir == Direction::RIGHT) {
            std::sort(blocks.begin(), blocks.end(), [](const Point& a, const Point& b) {
                return a.getX() < b.getX();
            });
        }
        else {
            std::sort(blocks.begin(), blocks.end(), [](const Point& a, const Point& b) {
                return a.getX() > b.getX();
            });
        }
    }
    else {
        if (releaseDir == Direction::DOWN) {
            std::sort(blocks.begin(), blocks.end(), [](const Point& a, const Point& b) {
                return a.getY() < b.getY();
            });
        }
        else {
            std::sort(blocks.begin(), blocks.end(), [](const Point& a, const Point& b) {
                return a.getY() > b.getY();
            });
        }
    }
}

bool Spring::containsPoint(const Point& p) const {
    return containsPoint(p.getX(), p.getY());
}

bool Spring::containsPoint(int x, int y) const {
    for (const auto& block : blocks) {
        if (block.getX() == x && block.getY() == y) {
            return true;
        }
    }
    return false;
}

// Check if block is still visible (not compressed away)
bool Spring::isBlockVisible(const Point& p) const {
    if (!containsPoint(p)) return false;

    for (int i = 0; i < (int)blocks.size(); i++) {
        if (blocks[i].getX() == p.getX() && blocks[i].getY() == p.getY()) {
            int visibleCount = originalLength - currentCompression;
            return i < visibleCount;
        }
    }
    return false;
}

// Get entry point (farthest block from wall)
Point Spring::getEntryPoint() const {
    if (blocks.empty()) return Point(0, 0);
    return blocks.back();
}

// Begin compression when player enters spring area
void Spring::onPlayerEnter(Player* player, const Point& pos) {
    if (!player) return;

    compressingPlayer = player;
    isCompressed = true;
    playerPosition = pos;
    currentCompression = 1;
}

// Increase compression when player moves toward wall
void Spring::onPlayerMove(Player* player, const Point& newPos, Direction moveDir) {
    if (!player || player != compressingPlayer) return;

    playerPosition = newPos;

    // Moving toward wall compresses more
    bool movingTowardWall = false;
    if (releaseDir == Direction::RIGHT && moveDir == Direction::LEFT) movingTowardWall = true;
    if (releaseDir == Direction::LEFT && moveDir == Direction::RIGHT) movingTowardWall = true;
    if (releaseDir == Direction::DOWN && moveDir == Direction::UP) movingTowardWall = true;
    if (releaseDir == Direction::UP && moveDir == Direction::DOWN) movingTowardWall = true;

    if (movingTowardWall) {
        currentCompression++;
        if (currentCompression > originalLength) {
            currentCompression = originalLength;
        }
    }
}

void Spring::onPlayerStay(Player* player) {
    // STAY triggers release check
}

void Spring::onPlayerExit(Player* player) {
    if (player == compressingPlayer) {
        compressingPlayer = nullptr;
        isCompressed = false;
        currentCompression = 0;
    }
}

// AI-assisted: Determine if spring should release based on player input
// Releases on: STAY key, direction change away from wall, or max compression
bool Spring::shouldRelease(Direction playerDir) const {
    if (!isCompressed || currentCompression == 0) return false;
    
    // STAY key always releases
    if (playerDir == Direction::STAY) return true;
    
    bool movingTowardWall = false;
    if (releaseDir == Direction::RIGHT && playerDir == Direction::LEFT) movingTowardWall = true;
    if (releaseDir == Direction::LEFT && playerDir == Direction::RIGHT) movingTowardWall = true;
    if (releaseDir == Direction::DOWN && playerDir == Direction::UP) movingTowardWall = true;
    if (releaseDir == Direction::UP && playerDir == Direction::DOWN) movingTowardWall = true;
    
    // Release when changing direction away from wall
    if (!movingTowardWall) return true;
    
    // Max compression also releases
    if (currentCompression >= originalLength) return true;
    
    return false;
}

// Calculate launch speed and duration from compression level
Spring::ReleaseInfo Spring::release() {
    ReleaseInfo info;
    info.released = false;
    info.speed = 0;
    info.duration = 0;
    info.direction = releaseDir;

    if (isCompressed && currentCompression > 0) {
        info.released = true;
        info.speed = currentCompression;
        info.duration = currentCompression * currentCompression;  // speed^2 for distance
        info.direction = releaseDir;
        hasBeenUsed = true;
        resetCompression();
    }

    return info;
}

void Spring::resetCompression() {
    isCompressed = false;
    currentCompression = 0;
    compressingPlayer = nullptr;
}

// Update visible spring blocks based on compression
void Spring::updateBoard(GameBoard* board) const {
    if (!board) return;

    int visibleCount = originalLength - currentCompression;

    for (int i = 0; i < (int)blocks.size(); i++) {
        if (i < visibleCount) {
            board->setCell(blocks[i].getX(), blocks[i].getY(), '#');
        }
        else {
            char current = board->getCell(blocks[i].getX(), blocks[i].getY());
            if (current == '#') {
                board->setCell(blocks[i].getX(), blocks[i].getY(), ' ');
            }
        }
    }
}

// Draw spring with color indicating state (cyan=ready, yellow=stressed, white=spent)
void Spring::draw() const {
    int visibleCount = originalLength - currentCompression;

    for (int i = 0; i < (int)blocks.size(); i++) {
        gotoxy(blocks[i].getX(), blocks[i].getY());

        if (i < visibleCount) {
            if (isCompressed) {
                ::setColor(Color::Yellow);   // stressed
            }
            else if (hasBeenUsed) {
                ::setColor(Color::White);    // spent
            }
            else {
                ::setColor(Color::Cyan);     // ready
            }
            std::cout << '#';
        }
    }
    resetColor();
}

// --- SpringManager ---

// AI-assisted: Detect wall attachment and set spring orientation/release direction
void SpringManager::analyzeSpring(Spring& spring, GameBoard* board) {
    const auto& springBlocks = spring.getBlocks();
    if (springBlocks.empty() || !board) return;

    bool horizontal = true;
    bool vertical = true;

    int firstY = springBlocks[0].getY();
    int firstX = springBlocks[0].getX();

    for (const auto& b : springBlocks) {
        if (b.getY() != firstY) horizontal = false;
        if (b.getX() != firstX) vertical = false;
    }

    if (horizontal) {
        spring.setOrientation(Direction::RIGHT);

        int minX = springBlocks[0].getX(), maxX = springBlocks[0].getX();
        for (const auto& b : springBlocks) {
            if (b.getX() < minX) minX = b.getX();
            if (b.getX() > maxX) maxX = b.getX();
        }

        char leftCell = board->getCell(minX - 1, firstY);
        char rightCell = board->getCell(maxX + 1, firstY);

        if (leftCell == 'W') {
            spring.setWallEnd(Point(minX, firstY));
            spring.setReleaseDirection(Direction::RIGHT);
        }
        else if (rightCell == 'W') {
            spring.setWallEnd(Point(maxX, firstY));
            spring.setReleaseDirection(Direction::LEFT);
        }
    }
    else if (vertical) {
        spring.setOrientation(Direction::DOWN);

        int minY = springBlocks[0].getY(), maxY = springBlocks[0].getY();
        for (const auto& b : springBlocks) {
            if (b.getY() < minY) minY = b.getY();
            if (b.getY() > maxY) maxY = b.getY();
        }

        char topCell = board->getCell(firstX, minY - 1);
        char bottomCell = board->getCell(firstX, maxY + 1);

        if (topCell == 'W') {
            spring.setWallEnd(Point(firstX, minY));
            spring.setReleaseDirection(Direction::DOWN);
        }
        else if (bottomCell == 'W') {
            spring.setWallEnd(Point(firstX, maxY));
            spring.setReleaseDirection(Direction::UP);
        }
    }

    spring.finalize();
}

// Build springs from grid by detecting connected # characters
void SpringManager::buildFromGrid(const std::vector<std::string>& grid, GameBoard* board) {
    springs.clear();

    if (grid.empty()) return;

    int height = (int)grid.size();
    int width = 0;
    for (const auto& row : grid) {
        if ((int)row.size() > width) width = (int)row.size();
    }

    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < (int)grid[y].size(); x++) {
            if (grid[y][x] == '#' && !visited[y][x]) {
                Spring spring;

                // Count horizontal extent
                int hCount = 0;
                for (int tx = x; tx < (int)grid[y].size() && grid[y][tx] == '#'; tx++) {
                    hCount++;
                }

                // Count vertical extent
                int vCount = 0;
                for (int ty = y; ty < height && (int)grid[ty].size() > x && grid[ty][x] == '#'; ty++) {
                    vCount++;
                }

                // Prefer longer direction
                if (hCount >= vCount && hCount > 0) {
                    for (int tx = x; tx < x + hCount; tx++) {
                        spring.addBlock(tx, y);
                        visited[y][tx] = true;
                    }
                }
                else if (vCount > 0) {
                    for (int ty = y; ty < y + vCount; ty++) {
                        spring.addBlock(x, ty);
                        visited[ty][x] = true;
                    }
                }

                if (spring.getBlocks().size() > 0) {
                    analyzeSpring(spring, board);
                    springs.push_back(spring);
                }
            }
        }
    }
}

Spring* SpringManager::getSpringAt(const Point& p) {
    return getSpringAt(p.getX(), p.getY());
}

Spring* SpringManager::getSpringAt(int x, int y) {
    for (auto& spring : springs) {
        if (spring.containsPoint(x, y)) {
            return &spring;
        }
    }
    return nullptr;
}

bool SpringManager::hasSpringAt(const Point& p) const {
    return hasSpringAt(p.getX(), p.getY());
}

bool SpringManager::hasSpringAt(int x, int y) const {
    for (const auto& spring : springs) {
        if (spring.containsPoint(x, y)) {
            return true;
        }
    }
    return false;
}

void SpringManager::updateBoard(GameBoard* board) const {
    for (const auto& spring : springs) {
        spring.updateBoard(board);
    }
}

void SpringManager::drawAll() const {
    for (const auto& spring : springs) {
        spring.draw();
    }
}
