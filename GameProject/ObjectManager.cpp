// ObjectManager.cpp - Central manager for all game objects
// Manages walls, keys, doors, gates, switches, torches, riddles, bombs, obstacles, springs
// AI usage: Flood-fill grouping (AI-assisted), explosion cleanup (AI-assisted)

#include "ObjectManager.h"
#include <cstdlib>

// Struct for player-dropped keys
ObjectManager::DroppedKeyItem::DroppedKeyItem(int x, int y)
    : position(x, y), collected(false) {
}

ObjectManager::ObjectManager() : board(nullptr) {}

void ObjectManager::setBoard(GameBoard* b) {
    board = b;
}

// Add single wall tile at position
void ObjectManager::addWall(int x, int y) {
    walls.push_back(std::make_unique<Wall>(x, y));
    if (board) {
        board->setCell(x, y, GameConfig::CHAR_WALL);
    }
}

// Add horizontal wall segment
void ObjectManager::addHorizontalWall(int x, int y, int length) {
    for (int i = 0; i < length; i++) {
        addWall(x + i, y);
    }
}

// Add vertical wall segment
void ObjectManager::addVerticalWall(int x, int y, int length) {
    for (int i = 0; i < length; i++) {
        addWall(x, y + i);
    }
}

void ObjectManager::addKey(int x, int y) {
    keys.push_back(std::make_unique<Key>(x, y));
    if (board) {
        board->setCell(x, y, GameConfig::CHAR_KEY);
    }
}

void ObjectManager::addCoin(int x, int y) {
    coins.push_back(std::make_unique<Coin>(x, y));
    if (board) {
        board->setCell(x, y, GameConfig::CHAR_COIN);
    }
}

void ObjectManager::addDoor(int x, int y, char doorNum, int targetScreen) {
    doors.push_back(std::make_unique<Door>(x, y, doorNum, 0, targetScreen));
    if (board) {
        board->setCell(x, y, doorNum);
    }
}

void ObjectManager::addHorizontalDoor(int x, int y, int length, char doorNum, int targetScreen) {
    for (int i = 0; i < length; i++) {
        addDoor(x + i, y, doorNum, targetScreen);
    }
}

void ObjectManager::addVerticalDoor(int x, int y, int length, char doorNum, int targetScreen) {
    for (int i = 0; i < length; i++) {
        addDoor(x, y + i, doorNum, targetScreen);
    }
}

void ObjectManager::addGate(int x, int y, int width, int height, char doorNum) {
    gates.push_back(Gate(x, y, width, height, doorNum));
}

// Add light switch with initial state
void ObjectManager::addLightSwitch(int x, int y, bool startOn) {
    lightSwitches.push_back(std::make_unique<LightSwitch>(x, y, startOn));
    if (board) {
        board->setCell(x, y, 'S');
    }
}

// Add gate-linked switch
void ObjectManager::addSwitch(int x, int y, char linkedGate, bool startOn) {
    switches.push_back(std::make_unique<Switch>(x, y, linkedGate, startOn));
    if (board) {
        board->setCell(x, y, startOn ? '/' : '\\');
    }
}

void ObjectManager::addTorch(int x, int y, int radius) {
    torches.push_back(std::make_unique<Torch>(x, y, radius));
    if (board) {
        board->setCell(x, y, GameConfig::CHAR_TORCH);
    }
}

void ObjectManager::addRiddle(int x, int y, const std::string& question,
    const std::vector<std::string>& options, char correctAnswer) {
    riddles.push_back(std::make_unique<Riddle>(x, y, question, options, correctAnswer));
    if (board) {
        board->setCell(x, y, '?');
    }
}

void ObjectManager::addBomb(int x, int y) {
    bombs.push_back(std::make_unique<Bomb>(x, y));
    if (board) {
        board->setCell(x, y, '@');
    }
}

// Queue obstacle cell for grouping in finalizeObstacles
void ObjectManager::addObstacleCell(int x, int y) {
    pendingObstacleCells.push_back(Point(x, y));
    if (board) {
        board->setCell(x, y, '*');
    }
}

// Queue spring cell for grouping in finalizeSprings
void ObjectManager::addSpringCell(int x, int y) {
    pendingSpringCells.push_back(Point(x, y));
    if (board) {
        board->setCell(x, y, '#');
    }
}

// AI-assisted: Group adjacent obstacle cells using flood-fill algorithm
// Connects orthogonally adjacent '*' cells into single pushable obstacles
void ObjectManager::finalizeObstacles() {
    std::vector<bool> used(pendingObstacleCells.size(), false);
    
    for (size_t i = 0; i < pendingObstacleCells.size(); i++) {
        if (used[i]) continue;
        
        auto obstacle = std::make_unique<Obstacle>(
            pendingObstacleCells[i].getX(), 
            pendingObstacleCells[i].getY()
        );
        used[i] = true;
        
        // Flood fill to find connected cells (no diagonals)
        std::vector<size_t> toCheck;
        toCheck.push_back(i);
        
        while (!toCheck.empty()) {
            size_t current = toCheck.back();
            toCheck.pop_back();
            
            Point currentPos = pendingObstacleCells[current];
            
            for (size_t j = 0; j < pendingObstacleCells.size(); j++) {
                if (used[j]) continue;
                
                Point otherPos = pendingObstacleCells[j];
                int dx = std::abs(currentPos.getX() - otherPos.getX());
                int dy = std::abs(currentPos.getY() - otherPos.getY());
                
                // Adjacent means sharing an edge
                if ((dx == 1 && dy == 0) || (dx == 0 && dy == 1)) {
                    obstacle->addBlock(otherPos);
                    used[j] = true;
                    toCheck.push_back(j);
                }
            }
        }
        
        obstacle->saveOriginalPositions();
        obstacles.push_back(std::move(obstacle));
    }
    
    pendingObstacleCells.clear();
}

// AI-assisted: Group spring cells and detect wall attachment for release direction
// Determines horizontal/vertical orientation and which end has the wall
void ObjectManager::finalizeSprings() {
    std::vector<bool> used(pendingSpringCells.size(), false);
    
    for (size_t i = 0; i < pendingSpringCells.size(); i++) {
        if (used[i]) continue;
        
        auto spring = std::make_unique<Spring>();
        spring->addBlock(pendingSpringCells[i].getX(), pendingSpringCells[i].getY());
        used[i] = true;
        
        // Group cells in same row or column
        Point startPos = pendingSpringCells[i];
        
        for (size_t j = 0; j < pendingSpringCells.size(); j++) {
            if (used[j]) continue;
            
            Point otherPos = pendingSpringCells[j];
            
            if (startPos.getX() == otherPos.getX() || 
                startPos.getY() == otherPos.getY()) {
                spring->addBlock(otherPos);
                used[j] = true;
            }
        }
        
        // Figure out which end has the wall and set release direction
        if (board) {
            const auto& blocks = spring->getBlocks();
            if (!blocks.empty()) {
                int firstY = blocks[0].getY();
                int firstX = blocks[0].getX();
                
                bool horizontal = true;
                for (const auto& b : blocks) {
                    if (b.getY() != firstY) horizontal = false;
                }
                
                if (horizontal) {
                    int minX = blocks[0].getX(), maxX = blocks[0].getX();
                    for (const auto& b : blocks) {
                        if (b.getX() < minX) minX = b.getX();
                        if (b.getX() > maxX) maxX = b.getX();
                    }
                    
                    char leftCell = board->getCell(minX - 1, firstY);
                    char rightCell = board->getCell(maxX + 1, firstY);
                    
                    spring->setOrientation(Direction::RIGHT);
                    if (leftCell == 'W') {
                        spring->setWallEnd(Point(minX, firstY));
                        spring->setReleaseDirection(Direction::RIGHT);
                    } else if (rightCell == 'W') {
                        spring->setWallEnd(Point(maxX, firstY));
                        spring->setReleaseDirection(Direction::LEFT);
                    }
                } else {
                    int minY = blocks[0].getY(), maxY = blocks[0].getY();
                    for (const auto& b : blocks) {
                        if (b.getY() < minY) minY = b.getY();
                        if (b.getY() > maxY) maxY = b.getY();
                    }
                    
                    char topCell = board->getCell(firstX, minY - 1);
                    char bottomCell = board->getCell(firstX, maxY + 1);
                    
                    spring->setOrientation(Direction::DOWN);
                    if (topCell == 'W') {
                        spring->setWallEnd(Point(firstX, minY));
                        spring->setReleaseDirection(Direction::DOWN);
                    } else if (bottomCell == 'W') {
                        spring->setWallEnd(Point(firstX, maxY));
                        spring->setReleaseDirection(Direction::UP);
                    }
                }
            }
        }
        
        spring->finalize();
        springs.push_back(std::move(spring));
    }
    
    pendingSpringCells.clear();
}

// Create dropped key at player position
void ObjectManager::dropKey(const Point& pos) {
    droppedKeys.push_back(DroppedKeyItem(pos.getX(), pos.getY()));
    if (board) {
        board->setCell(pos, GameConfig::CHAR_KEY);
    }
}

// Create dropped torch at player position
void ObjectManager::dropTorch(const Point& pos, int radius) {
    droppedTorches.push_back(DroppedTorch(pos.getX(), pos.getY(), radius));
    if (board) {
        board->setCell(pos, GameConfig::CHAR_TORCH);
    }
}

// Object lookup - returns object at position or nullptr
Key* ObjectManager::getKeyAt(const Point& p) {
    for (auto& key : keys) {
        if (key->isActive() && key->collidesWith(p)) {
            return key.get();
        }
    }
    return nullptr;
}

Coin* ObjectManager::getCoinAt(const Point& p) {
    for (auto& coin : coins) {
        if (coin->isActive() && coin->collidesWith(p)) {
            return coin.get();
        }
    }
    return nullptr;
}

Door* ObjectManager::getDoorAt(const Point& p) {
    for (auto& door : doors) {
        if (door->isActive() && door->collidesWith(p)) {
            return door.get();
        }
    }
    return nullptr;
}

LightSwitch* ObjectManager::getLightSwitchAt(const Point& p) {
    for (auto& sw : lightSwitches) {
        if (sw->isActive() && sw->collidesWith(p)) {
            return sw.get();
        }
    }
    return nullptr;
}

Switch* ObjectManager::getSwitchAt(const Point& p) {
    for (auto& sw : switches) {
        if (sw->isActive() && sw->collidesWith(p)) {
            return sw.get();
        }
    }
    return nullptr;
}

Torch* ObjectManager::getTorchAt(const Point& p) {
    for (auto& torch : torches) {
        if (torch->isActive() && !torch->isCollected() && torch->collidesWith(p)) {
            return torch.get();
        }
    }
    return nullptr;
}

Gate* ObjectManager::getGateAt(const Point& p) {
    for (auto& gate : gates) {
        if (!gate.getIsOpen() && gate.isOnGateBar(p)) {
            return &gate;
        }
    }
    return nullptr;
}

Gate* ObjectManager::getGateByDoorNum(char doorNum) {
    for (auto& gate : gates) {
        if (gate.getDoorNum() == doorNum) {
            return &gate;
        }
    }
    return nullptr;
}

// Check if all switches linked to gate are in ON state
bool ObjectManager::areAllSwitchesOnForGate(char doorNum) const {
    bool hasSwitch = false;
    for (const auto& sw : switches) {
        if (sw->isActive() && sw->getLinkedGate() == doorNum) {
            hasSwitch = true;
            if (!sw->getIsOn()) {
                return false;
            }
        }
    }
    return hasSwitch;
}

ObjectManager::DroppedKeyItem* ObjectManager::getDroppedKeyAt(const Point& p) {
    for (auto& dk : droppedKeys) {
        if (!dk.collected && dk.position == p) {
            return &dk;
        }
    }
    return nullptr;
}

DroppedTorch* ObjectManager::getDroppedTorchAt(const Point& p) {
    for (auto& dt : droppedTorches) {
        if (!dt.collected && dt.position == p) {
            return &dt;
        }
    }
    return nullptr;
}

Riddle* ObjectManager::getRiddleAt(const Point& p) {
    for (auto& riddle : riddles) {
        if (riddle->isActive() && riddle->collidesWith(p)) {
            return riddle.get();
        }
    }
    return nullptr;
}

Bomb* ObjectManager::getBombAt(const Point& p) {
    for (auto& bomb : bombs) {
        if (bomb->isIdle() && bomb->collidesWith(p)) {
            return bomb.get();
        }
    }
    return nullptr;
}

Obstacle* ObjectManager::getObstacleAt(const Point& p) {
    for (auto& obstacle : obstacles) {
        if (obstacle->isActive() && obstacle->containsPoint(p)) {
            return obstacle.get();
        }
    }
    return nullptr;
}

Spring* ObjectManager::getSpringAt(const Point& p) {
    for (auto& spring : springs) {
        if (spring->isActive() && spring->containsPoint(p)) {
            return spring.get();
        }
    }
    return nullptr;
}

std::vector<std::unique_ptr<Bomb>>& ObjectManager::getBombs() {
    return bombs;
}

std::vector<std::unique_ptr<Obstacle>>& ObjectManager::getObstacles() {
    return obstacles;
}

std::vector<std::unique_ptr<Spring>>& ObjectManager::getSprings() {
    return springs;
}

// Check if uncollected keys exist in the level
bool ObjectManager::hasKeysInRoom() const {
    for (const auto& key : keys) {
        if (key->isActive() && !key->isCollected()) {
            return true;
        }
    }
    for (const auto& dk : droppedKeys) {
        if (!dk.collected) {
            return true;
        }
    }
    return false;
}

// Sync all object positions to game board cells
void ObjectManager::updateBoard() {
    if (!board) return;

    board->clear();

    for (const auto& wall : walls) {
        if (wall->isActive()) {
            board->setCell(wall->getX(), wall->getY(), wall->getSymbol());
        }
    }

    for (const auto& key : keys) {
        if (key->isActive() && !key->isCollected()) {
            board->setCell(key->getX(), key->getY(), key->getSymbol());
        }
    }

    for (const auto& coin : coins) {
        if (coin->isActive() && !coin->isCollected()) {
            board->setCell(coin->getX(), coin->getY(), coin->getSymbol());
        }
    }

    for (const auto& door : doors) {
        if (door->isActive()) {
            board->setCell(door->getX(), door->getY(), door->getDoorNum());
        }
    }

    for (const auto& sw : lightSwitches) {
        if (sw->isActive()) {
            board->setCell(sw->getX(), sw->getY(), sw->getSymbol());
        }
    }

    for (const auto& sw : switches) {
        if (sw->isActive()) {
            board->setCell(sw->getX(), sw->getY(), sw->getSymbol());
        }
    }

    for (const auto& torch : torches) {
        if (torch->isActive() && !torch->isCollected()) {
            board->setCell(torch->getX(), torch->getY(), torch->getSymbol());
        }
    }

    for (const auto& gate : gates) {
        gate.updateBoard(board->getRawBoard());
    }

    for (const auto& dk : droppedKeys) {
        if (!dk.collected) {
            board->setCell(dk.position, GameConfig::CHAR_KEY);
        }
    }

    for (const auto& dt : droppedTorches) {
        if (!dt.collected) {
            board->setCell(dt.position, GameConfig::CHAR_TORCH);
        }
    }

    for (const auto& riddle : riddles) {
        if (riddle->isActive()) {
            board->setCell(riddle->getX(), riddle->getY(), riddle->getSymbol());
        }
    }
    
    for (const auto& bomb : bombs) {
        if (bomb->isIdle()) {
            board->setCell(bomb->getX(), bomb->getY(), '@');
        }
    }
    
    for (const auto& obstacle : obstacles) {
        if (obstacle->isActive()) {
            obstacle->updateBoard(board);
        }
    }
    
    for (const auto& spring : springs) {
        if (spring->isActive()) {
            spring->updateBoard(board);
        }
    }
}

// Reset all objects to initial state for level restart
void ObjectManager::reset() {
    for (auto& key : keys) { key->reset(); }
    for (auto& coin : coins) { coin->reset(); }
    for (auto& door : doors) { door->reset(); }
    for (auto& sw : lightSwitches) { sw->reset(); }
    for (auto& sw : switches) { sw->reset(); }
    for (auto& torch : torches) { torch->reset(); }
    for (auto& gate : gates) { gate.reset(); }
    for (auto& riddle : riddles) { riddle->activate(); }
    for (auto& bomb : bombs) { bomb->resetBomb(); }
    for (auto& obstacle : obstacles) { obstacle->reset(); }
    for (auto& spring : springs) { spring->resetCompression(); }
    droppedKeys.clear();
    droppedTorches.clear();
}

// Lighting system accessors
const std::vector<std::unique_ptr<LightSwitch>>* ObjectManager::getLightSwitches() const {
    return &lightSwitches;
}

const std::vector<std::unique_ptr<Torch>>* ObjectManager::getTorches() const {
    return &torches;
}

const std::vector<DroppedTorch>* ObjectManager::getDroppedTorches() const {
    return &droppedTorches;
}

// AI-assisted: Destroy objects at position (explosion cleanup)
// Removes walls, switches, torches, keys, riddles, bombs, obstacles, springs, and gates
// Doors are protected from destruction as they are level exits
bool ObjectManager::destroyAt(const Point& p) {
    bool destroyed = false;

    auto clearCell = [&](const Point& pt) {
        if (board) board->setCell(pt, GameConfig::CHAR_EMPTY);
    };

    // Walls can be destroyed
    for (auto& wall : walls) {
        if (wall->isActive() && wall->collidesWith(p)) {
            wall->deactivate();
            clearCell(p);
            destroyed = true;
        }
    }

    // Doors are protected from explosions (level exits)

    for (auto& sw : lightSwitches) {
        if (sw->isActive() && sw->collidesWith(p)) {
            sw->deactivate();
            clearCell(p);
            destroyed = true;
        }
    }

    for (auto& sw : switches) {
        if (sw->isActive() && sw->collidesWith(p)) {
            sw->deactivate();
            clearCell(p);
            destroyed = true;
        }
    }

    for (auto& torch : torches) {
        if (torch->isActive() && torch->collidesWith(p)) {
            torch->deactivate();
            clearCell(p);
            destroyed = true;
        }
    }

    for (auto& key : keys) {
        if (key->isActive() && key->collidesWith(p)) {
            key->deactivate();
            clearCell(p);
            destroyed = true;
        }
    }

    for (auto& riddle : riddles) {
        if (riddle->isActive() && riddle->collidesWith(p)) {
            riddle->deactivate();
            clearCell(p);
            destroyed = true;
        }
    }

    for (auto& bomb : bombs) {
        if (bomb->isIdle() && bomb->collidesWith(p)) {
            bomb->deactivate();
            clearCell(p);
            destroyed = true;
        }
    }

    // Destroying any part destroys the whole obstacle
    for (auto& obs : obstacles) {
        if (obs->isActive() && obs->containsPoint(p)) {
            obs->clearFromBoard(board);
            obs->deactivate();
            destroyed = true;
        }
    }

    for (auto& spring : springs) {
        if (spring->isActive() && spring->containsPoint(p)) {
            spring->deactivate();
            destroyed = true;
        }
    }

    for (auto& dk : droppedKeys) {
        if (!dk.collected && dk.position == p) {
            dk.collected = true;
            clearCell(p);
            destroyed = true;
        }
    }

    for (auto& dt : droppedTorches) {
        if (!dt.collected && dt.position == p) {
            dt.collected = true;
            clearCell(p);
            destroyed = true;
        }
    }

    // Hit any part of gate removes the whole thing
    for (auto it = gates.begin(); it != gates.end(); ) {
        bool hit = it->isOnGateBar(p) || it->isAtCorner(p.getX(), p.getY());
        if (hit) {
            if (board) {
                it->clearBarsFromBoard(board->getRawBoard());
                board->setCell(Point(it->getX(), it->getY()), GameConfig::CHAR_EMPTY);
                board->setCell(Point(it->getX() + it->getWidth() - 1, it->getY()), GameConfig::CHAR_EMPTY);
                board->setCell(Point(it->getX(), it->getY() + it->getHeight() - 1), GameConfig::CHAR_EMPTY);
                board->setCell(Point(it->getX() + it->getWidth() - 1, it->getY() + it->getHeight() - 1), GameConfig::CHAR_EMPTY);
            }
            it = gates.erase(it);
            destroyed = true;
        } else {
            ++it;
        }
    }

    return destroyed;
}
