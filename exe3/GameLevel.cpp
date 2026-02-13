// GameLevel.cpp - Core gameplay logic for a single level
// Contains main update loop with spring momentum physics, bomb explosions, and shrapnel
// AI usage: Spring momentum physics (AI written), player movement logic (AI assisted),
//           bomb chain reactions (AI written), shrapnel animation (AI written)

#include "GameLevel.h"
#include "Game.h"
#include "GameRecorder.h"
#include "LivesManager.h"
#include "console.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <set>
#include <string>

// Constructor: Wire up all subsystems and seed random for heart spawning
GameLevel::GameLevel(int num)
    : levelNumber(num), result(ScreenResult::None), needsFullRedraw(true),
    silentMode(false), player1(nullptr), player2(nullptr),
    heartActive(false), heartRespawnTimer(HEART_RESPAWN_DELAY),
    player1CrossedDoor(false), player2CrossedDoor(false), doorTargetScreen(-1) {
    statusBar.setLevelNumber(num);
    objects.setBoard(&board);
    collision.setBoard(&board);
    collision.setObjectManager(&objects);
    interactions.setBoard(&board);
    interactions.setObjectManager(&objects);
    interactions.setLightingSystem(&lighting);
    interactions.setStatusBar(&statusBar);
    interactions.setMessageDisplay(&messageDisplay);
    interactions.setGame(game);
    renderer.setBoard(&board);
    renderer.setLightingSystem(&lighting);

    // Seed random once for heart spawning
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(nullptr));
        seeded = true;
    }
}

// Connect player pointers to all systems that need them
void GameLevel::setPlayers(Player* p1, Player* p2) {
    player1 = p1;
    player2 = p2;
    statusBar.setPlayers(p1, p2);
    lighting.setPlayers(p1, p2);
    input.setPlayers(p1, p2);
    renderer.setPlayers(p1, p2);
}

// Link game instance and pass recorder for save/load
void GameLevel::setGame(Game* g) {
    game = g;
    if (game) {
        input.setRecorder(&game->getRecorder());
        interactions.setRecorder(&game->getRecorder());
    }
}

// Enable silent mode for automated testing
void GameLevel::setSilentMode(bool silent) {
    silentMode = silent;
    renderer.setSilentMode(silent);
}

// Object creation delegates - forward to ObjectManager

void GameLevel::addWall(int x, int y) { objects.addWall(x, y); }
void GameLevel::addHorizontalWall(int x, int y, int length) { objects.addHorizontalWall(x, y, length); }
void GameLevel::addVerticalWall(int x, int y, int length) { objects.addVerticalWall(x, y, length); }
void GameLevel::addKey(int x, int y) { objects.addKey(x, y); }

void GameLevel::addDoor(int x, int y, char doorNum, int targetScreen) {
    objects.addDoor(x, y, doorNum, targetScreen);
}

void GameLevel::addHorizontalDoor(int x, int y, int length, char doorNum, int targetScreen) {
    objects.addHorizontalDoor(x, y, length, doorNum, targetScreen);
}

void GameLevel::addVerticalDoor(int x, int y, int length, char doorNum, int targetScreen) {
    objects.addVerticalDoor(x, y, length, doorNum, targetScreen);
}

void GameLevel::addGate(int x, int y, int width, int height, char doorNum) {
    objects.addGate(x, y, width, height, doorNum);
}

void GameLevel::addLightSwitch(int x, int y, bool startOn) { objects.addLightSwitch(x, y, startOn); }
void GameLevel::addSwitch(int x, int y, char linkedGate, bool startOn) { objects.addSwitch(x, y, linkedGate, startOn); }
void GameLevel::addTorch(int x, int y, int radius) { objects.addTorch(x, y, radius); }

void GameLevel::addRiddle(int x, int y, const std::string& question,
    const std::vector<std::string>& options, char correctAnswer) {
    objects.addRiddle(x, y, question, options, correctAnswer);
}

void GameLevel::addBomb(int x, int y) { objects.addBomb(x, y); }
void GameLevel::addCoin(int x, int y) { objects.addCoin(x, y); }
void GameLevel::addObstacleCell(int x, int y) { objects.addObstacleCell(x, y); }
void GameLevel::addSpringCell(int x, int y) { objects.addSpringCell(x, y); }

// Lighting configuration
void GameLevel::setDarkRoom(bool dark) { lighting.setDarkRoom(dark); }
bool GameLevel::getIsDarkRoom() const { return lighting.getIsDarkRoom(); }
void GameLevel::setTorchRadius(int radius) { lighting.setTorchRadius(radius); }

// Check if both players reached the exit door
bool GameLevel::checkBothPlayersOnDoor() const {
    if (!player1 || !player2) return false;
    return board.isDoor(player1->getPosition()) && board.isDoor(player2->getPosition());
}

// Finalize obstacles/springs and connect lighting after objects are added
void GameLevel::init() {
    objects.finalizeObstacles();
    objects.finalizeSprings();

    lighting.setLightSwitches(objects.getLightSwitches());
    lighting.setTorches(objects.getTorches());
    lighting.setDroppedTorches(objects.getDroppedTorches());

    objects.updateBoard();
}

void GameLevel::enter() {
    BaseScreen::enter();
    result = ScreenResult::None;
    needsFullRedraw = true;
    messageDisplay.clear();
    if (!silentMode) {
        clrscr();
    }
}

void GameLevel::exit() {
    BaseScreen::exit();
}

// Reset level to initial state for retry or new game
void GameLevel::reset() {
    result = ScreenResult::None;
    needsFullRedraw = true;
    messageDisplay.clear();
    pauseMenu.reset();
    input.setPaused(false);

    heartActive = false;
    heartRespawnTimer = HEART_RESPAWN_DELAY;
    player1CrossedDoor = false;
    player2CrossedDoor = false;
    doorTargetScreen = -1;

    objects.reset();
    objects.updateBoard();
}

// Process keyboard input for pause menu, save, and item drops
void GameLevel::handleInput() {
    input.setPaused(pauseMenu.getIsPaused());
    InputResult inputResult = input.processInput();

    if (inputResult.escPressed) {
        pauseMenu.toggle();
        needsFullRedraw = true;
        return;
    }

    if (inputResult.menuRequested) {
        result = ScreenResult::ExitToMenu;
        return;
    }

    if (inputResult.saveRequested) {
        // Delegate save to Game class
        if (game) {
            game->saveCurrentState();
            messageDisplay.show("Game Saved!");
        }
        needsFullRedraw = true;
        return;
    }

    // Player 1 drops item
    if (inputResult.player1Dispose && player1) {
        interactions.dropPlayerItem(*player1);
        if (player1->hasTorch()) needsFullRedraw = true;
        renderer.drawCell(player1->getPosition().getX(), player1->getPosition().getY());
    }

    // Player 2 drops item
    if (inputResult.player2Dispose && player2) {
        interactions.dropPlayerItem(*player2);
        if (player2->hasTorch()) needsFullRedraw = true;
        renderer.drawCell(player2->getPosition().getX(), player2->getPosition().getY());
    }
}

// AI-assisted: Player movement handling with gates, riddles, obstacles, and springs
// Handles spring compression/release, gate key requirements, riddle blocking,
// obstacle collision, and standard movement with interaction callbacks
void GameLevel::handlePlayerMovement(Player& player, Player* otherPlayer) {
    // Check spring release for stationary players (STAY key release)
    // This must happen BEFORE canMoveThisFrame check to support STAY key
    if (!player.getIsMoving()) {
        Spring* spring = objects.getSpringAt(player.getPosition());
        if (spring && spring->getIsCompressed() && spring->getCompressingPlayer() == &player) {
            Direction dir = player.getDirection();  // Will be STAY
            if (spring->shouldRelease(dir)) {
                Spring::ReleaseInfo info = spring->release();
                if (info.released) {
                    player.setSpringMomentum(info.direction, info.speed, info.duration);
                    messageDisplay.show("Spring launched! Speed: " + std::to_string(info.speed));
                }
                spring->updateBoard(&board);
                needsFullRedraw = true;
            }
        }
        return;
    }
    
    if (!player.canMoveThisFrame()) return;

    // Ignore other player if they've finished (crossed door)
    if (otherPlayer == player1 && player1CrossedDoor) otherPlayer = nullptr;
    else if (otherPlayer == player2 && player2CrossedDoor) otherPlayer = nullptr;

    Point nextPos = player.getNextPosition();

    // Gate interaction - need key
    Gate* gate = objects.getGateAt(nextPos);
    if (gate && !gate->getIsOpen()) {
        if (player.hasKey()) {
            interactions.tryOpenGate(player, nextPos);
            // Redraw gate perimeter
            int gx = gate->getX(), gy = gate->getY();
            int gw = gate->getWidth(), gh = gate->getHeight();
            for (int i = 0; i < gw; i++) {
                renderer.drawCell(gx + i, gy);
                renderer.drawCell(gx + i, gy + gh - 1);
            }
            for (int i = 1; i < gh - 1; i++) {
                renderer.drawCell(gx, gy + i);
                renderer.drawCell(gx + gw - 1, gy + i);
            }
        } else {
            messageDisplay.show("You need a KEY to open this gate!");
            player.stop();
        }
        return;
    }

    // Riddle blocks both players until answered
    Riddle* riddle = objects.getRiddleAt(nextPos);
    if (riddle && riddle->isActive()) {
        player.stop();
        if (otherPlayer) otherPlayer->stop();

        int playerNum = (&player == player1) ? 1 : 2;
        bool correct = interactions.handleRiddle(player, *riddle, playerNum);
        ::markRiddleUsed(riddle->getQuestion());
        
        // Record riddle event
        if (game) {
            char answer = interactions.getLastRiddleAnswer();
            game->getRecorder().recordRiddle(playerNum, riddle->getQuestion(), 
                                             answer, correct);
        }

        if (correct) {
            riddle->deactivate();
            board.setCell(nextPos.getX(), nextPos.getY(), ' ');
            needsFullRedraw = true;
        } else {
            // Record life lost
            if (game) {
                game->getRecorder().recordLifeLost(playerNum);
            }
            if (!LivesManager::hasLivesRemaining()) {
                result = ScreenResult::GameOver;
            }
            needsFullRedraw = true;
        }
        return;
    }

    // Obstacle pushing
    Obstacle* obstacle = objects.getObstacleAt(nextPos);
    if (obstacle) {
        Direction dir = player.getDirection();
        if (!tryPushObstacle(player, otherPlayer, dir)) {
            player.stop();
            return;
        }
        needsFullRedraw = true;
    }

    // Standard collision check
    if (collision.checkMovementCollision(nextPos, otherPlayer)) {
        // Transfer momentum on player collision
        if (player.hasActiveSpringMomentum() && otherPlayer && otherPlayer->getPosition() == nextPos) {
            otherPlayer->setSpringMomentum(
                player.getSpringDirection(),
                player.getSpringSpeed(),
                player.getSpringSpeed() * player.getSpringSpeed()
            );
        }
        
        // Check spring release when blocked by wall (max compression release)
        Spring* spring = objects.getSpringAt(player.getPosition());
        if (spring && spring->getIsCompressed() && spring->getCompressingPlayer() == &player) {
            Direction dir = player.getDirection();
            if (spring->shouldRelease(dir)) {
                Spring::ReleaseInfo info = spring->release();
                if (info.released) {
                    player.setSpringMomentum(info.direction, info.speed, info.duration);
                    messageDisplay.show("Spring launched! Speed: " + std::to_string(info.speed));
                }
                spring->updateBoard(&board);
                needsFullRedraw = true;
            }
        }
        
        player.stop();
        return;
    }

    // Actually move
    Point oldPos = player.getPosition();
    player.erase();
    renderer.drawCell(oldPos.getX(), oldPos.getY());
    player.move();

    // Did player reach door?
    if (board.isDoor(player.getPosition())) {
        Point doorPos = player.getPosition();
        player.erase();
        renderer.drawCell(doorPos.getX(), doorPos.getY());
        
        // Store the door's target screen
        Door* door = objects.getDoorAt(doorPos);
        if (door && door->getTargetScreen() != -1) {
            doorTargetScreen = door->getTargetScreen();
        }
        
        if (&player == player1) player1CrossedDoor = true;
        else if (&player == player2) player2CrossedDoor = true;
        return;
    }

    handleSpringInteraction(player, otherPlayer);

    // Heart pickup
    if (heartActive && player.getPosition() == heart.getPosition()) {
        LivesManager::addLife();
        messageDisplay.show("+1 Life!");
        despawnHeart();
        needsFullRedraw = true;
    }

    // Item pickups etc
    InteractionResult interactionResult = interactions.handleInteractions(player);
    if (interactionResult.gameOver) {
        result = ScreenResult::GameOver;
        return;
    }
    if (interactionResult.needsFullRedraw) needsFullRedraw = true;
    else if (interactionResult.needsRedraw) renderer.drawCell(player.getPosition().getX(), player.getPosition().getY());

    // Redraw torch light in dark rooms
    if (lighting.getIsDarkRoom() && !lighting.isRoomLit() && player.hasTorch()) {
        int radius = lighting.getTorchRadius() + 2;
        renderer.redrawTorchArea(player.getPosition().getX(), player.getPosition().getY(), radius);
    }
}

// AI-written: Main game loop update with spring momentum physics
// Handles spring-launched movement with multi-cell traversal, momentum transfer
// between players on collision, perpendicular steering during spring flight,
// and cooperative obstacle pushing during momentum. Complex direction tracking
// and force calculation for physics-accurate spring behavior.
void GameLevel::update() {
    if (result != ScreenResult::None) return;
    if (pauseMenu.getIsPaused()) return;

    messageDisplay.update();
    updateHeart();
    updateBombs();
    updateShrapnel();

    // Player 1 movement (spring momentum overrides normal movement)
    if (player1) {
        if (player1->hasActiveSpringMomentum()) {
            Direction springDir = player1->getSpringDirection();
            Direction inputDir = player1->getDirection();
            int speed = player1->getSpringSpeed();
            int duration = player1->getSpringSpeed() * player1->getSpringSpeed();
            
            // Classify input relative to spring direction
            bool isBackward = false;
            if (springDir == Direction::RIGHT && inputDir == Direction::LEFT) isBackward = true;
            if (springDir == Direction::LEFT && inputDir == Direction::RIGHT) isBackward = true;
            if (springDir == Direction::UP && inputDir == Direction::DOWN) isBackward = true;
            if (springDir == Direction::DOWN && inputDir == Direction::UP) isBackward = true;
            
            bool isLateral = false;
            if ((springDir == Direction::LEFT || springDir == Direction::RIGHT) &&
                (inputDir == Direction::UP || inputDir == Direction::DOWN)) isLateral = true;
            if ((springDir == Direction::UP || springDir == Direction::DOWN) &&
                (inputDir == Direction::LEFT || inputDir == Direction::RIGHT)) isLateral = true;
            
            // Move at spring speed in spring direction
            player1->setDirection(springDir);
            for (int i = 0; i < speed; i++) {
                Point nextPos = player1->getNextPosition();
                
                if (!nextPos.isInBounds()) break;
                
                char cell = board.getCell(nextPos);
                if (cell == 'W') break;
                
                // Transfer momentum on collision
                if (player2 && nextPos == player2->getPosition()) {
                    player2->setSpringMomentum(springDir, speed, duration);
                    messageDisplay.show("Momentum transferred!");
                    break;
                }
                
                // Try cooperative push
                Obstacle* obstacle = objects.getObstacleAt(nextPos);
                if (obstacle) {
                    int force = player1->getForce();

                    if (player2) {
                        Point otherPos = player2->getPosition();
                        int dx = std::abs(player1->getPosition().getX() - otherPos.getX());
                        int dy = std::abs(player1->getPosition().getY() - otherPos.getY());
                        bool playersAdjacent = (dx == 1 && dy == 0) || (dx == 0 && dy == 1);
                        Point otherNextPos = otherPos.getNextPosition(springDir);
                        bool otherAdjacentToObstacle = obstacle->containsPoint(otherNextPos);

                        if (playersAdjacent || otherAdjacentToObstacle) {
                            bool otherHasSpringMomentum = player2->hasActiveSpringMomentum() &&
                                                         player2->getSpringDirection() == springDir;
                            bool sameDirection = (player2->getDirection() == springDir);
                            bool recentlySameDirection = (player2->getLastMoveDirection() == springDir &&
                                                          player2->getLastMoveAge() <= 5);

                            if (otherHasSpringMomentum || sameDirection || recentlySameDirection) {
                                force += player2->getForce();
                            }
                        }
                    }

                    if (obstacle->canPush(springDir, &board, force)) {
                        obstacle->push(springDir, &board, force);
                        needsFullRedraw = true;
                    } else {
                        break;
                    }
                }
                
                if (cell == '*' || cell == '|' || cell == '-') break;
                
                player1->erase();
                renderer.drawCell(player1->getPosition().getX(), player1->getPosition().getY());
                player1->move();
            }
            
            // Allow perpendicular steering while in spring momentum
            if (isLateral && !isBackward) {
                player1->setDirection(inputDir);
                Point nextPos = player1->getNextPosition();
                Player* p2ForCollision = player2CrossedDoor ? nullptr : player2;
                
                if (nextPos.isInBounds() && !collision.checkMovementCollision(nextPos, p2ForCollision)) {
                    char cell = board.getCell(nextPos);
                    if (cell != 'W' && cell != '*' && cell != '|' && cell != '-') {
                        player1->erase();
                        renderer.drawCell(player1->getPosition().getX(), player1->getPosition().getY());
                        player1->move();
                    }
                }
            }
            
        } else {
            handlePlayerMovement(*player1, player2);
        }
        player1->updateSpringMomentum();
        player1->updateLastMoveAge();
    }

    // Player 2 movement (same logic as player 1)
    if (player2) {
        if (player2->hasActiveSpringMomentum()) {
            Direction springDir = player2->getSpringDirection();
            Direction inputDir = player2->getDirection();
            int speed = player2->getSpringSpeed();
            int duration = player2->getSpringSpeed() * player2->getSpringSpeed();
            
            bool isBackward = false;
            if (springDir == Direction::RIGHT && inputDir == Direction::LEFT) isBackward = true;
            if (springDir == Direction::LEFT && inputDir == Direction::RIGHT) isBackward = true;
            if (springDir == Direction::UP && inputDir == Direction::DOWN) isBackward = true;
            if (springDir == Direction::DOWN && inputDir == Direction::UP) isBackward = true;
            
            bool isLateral = false;
            if ((springDir == Direction::LEFT || springDir == Direction::RIGHT) &&
                (inputDir == Direction::UP || inputDir == Direction::DOWN)) isLateral = true;
            if ((springDir == Direction::UP || springDir == Direction::DOWN) &&
                (inputDir == Direction::LEFT || inputDir == Direction::RIGHT)) isLateral = true;
            
            player2->setDirection(springDir);
            for (int i = 0; i < speed; i++) {
                Point nextPos = player2->getNextPosition();
                
                if (!nextPos.isInBounds()) break;
                
                char cell = board.getCell(nextPos);
                if (cell == 'W') break;
                
                if (player1 && nextPos == player1->getPosition()) {
                    player1->setSpringMomentum(springDir, speed, duration);
                    messageDisplay.show("Momentum transferred!");
                    break;
                }
                
                Obstacle* obstacle = objects.getObstacleAt(nextPos);
                if (obstacle) {
                    int force = player2->getForce();

                    if (player1) {
                        Point otherPos = player1->getPosition();
                        int dx = std::abs(player2->getPosition().getX() - otherPos.getX());
                        int dy = std::abs(player2->getPosition().getY() - otherPos.getY());
                        bool playersAdjacent = (dx == 1 && dy == 0) || (dx == 0 && dy == 1);
                        Point otherNextPos = otherPos.getNextPosition(springDir);
                        bool otherAdjacentToObstacle = obstacle->containsPoint(otherNextPos);

                        if (playersAdjacent || otherAdjacentToObstacle) {
                            bool otherHasSpringMomentum = player1->hasActiveSpringMomentum() &&
                                                         player1->getSpringDirection() == springDir;
                            bool sameDirection = (player1->getDirection() == springDir);
                            bool recentlySameDirection = (player1->getLastMoveDirection() == springDir &&
                                                          player1->getLastMoveAge() <= 5);

                            if (otherHasSpringMomentum || sameDirection || recentlySameDirection) {
                                force += player1->getForce();
                            }
                        }
                    }

                    if (obstacle->canPush(springDir, &board, force)) {
                        obstacle->push(springDir, &board, force);
                        needsFullRedraw = true;
                    } else {
                        break;
                    }
                }
                
                if (cell == '*' || cell == '|' || cell == '-') break;
                
                player2->erase();
                renderer.drawCell(player2->getPosition().getX(), player2->getPosition().getY());
                player2->move();
            }
            
            if (isLateral && !isBackward) {
                player2->setDirection(inputDir);
                Point nextPos = player2->getNextPosition();
                Player* p1ForCollision = player1CrossedDoor ? nullptr : player1;
                
                if (nextPos.isInBounds() && !collision.checkMovementCollision(nextPos, p1ForCollision)) {
                    char cell = board.getCell(nextPos);
                    if (cell != 'W' && cell != '*' && cell != '|' && cell != '-') {
                        player2->erase();
                        renderer.drawCell(player2->getPosition().getX(), player2->getPosition().getY());
                        player2->move();
                    }
                }
            }
            
        } else {
            handlePlayerMovement(*player2, player1);
        }
        player2->updateSpringMomentum();
        player2->updateLastMoveAge();
    }
    
    // Keep spring visuals updated
    for (auto& spring : objects.getSprings()) {
        spring->updateBoard(&board);
    }

    // Both finished? Level complete
    if (player1CrossedDoor && player2CrossedDoor) {
        result = ScreenResult::NextScreen;
    }
}

// Render level, UI elements, and players (skipped in silent mode)
void GameLevel::render() {
    // Skip rendering in silent mode
    if (silentMode) {
        needsFullRedraw = false;
        return;
    }
    
    if (needsFullRedraw) {
        renderer.drawBoard();
        statusBar.setDarkRoomStatus(lighting.getIsDarkRoom(), lighting.isRoomLit());
        statusBar.draw();
        needsFullRedraw = false;
    }

    if (heartActive) heart.draw();

    for (auto& bomb : objects.getBombs()) {
        if (bomb->isActive()) bomb->draw();
    }

    for (auto& spring : objects.getSprings()) {
        spring->draw();
    }

    // Shrapnel particles
    for (const auto& p : shrapnelParticles) {
        if (p.active) {
            int x = p.pos.getX();
            int y = p.pos.getY();
            bool isVisible = !lighting.getIsDarkRoom() || lighting.isCellVisible(x, y);
            if (isVisible) {
                gotoxy(x, y);
                ::setColor(Color::LightRed);
                std::cout << p.sym;
                resetColor();
            }
        }
    }

    // Draw players unless they've exited
    if (player1 && !player1CrossedDoor) player1->draw();
    if (player2 && !player2CrossedDoor) player2->draw();
    messageDisplay.draw();

    if (pauseMenu.getIsPaused()) pauseMenu.draw();

    std::cout.flush();
}

ScreenResult GameLevel::getResult() const { return result; }
int GameLevel::getLevelNumber() const { return levelNumber; }
int GameLevel::getDoorTargetScreen() const { return doorTargetScreen; }

// Riddle management - forward to ObjectManager
std::vector<std::unique_ptr<Riddle>>& GameLevel::getRiddles() { return objects.getRiddles(); }
void GameLevel::clearRiddles() { objects.clearRiddles(); }

void GameLevel::addRiddle(std::unique_ptr<Riddle> riddle) {
    if (riddle) {
        objects.addRiddle(riddle->getX(), riddle->getY(),
                         riddle->getQuestion(), riddle->getOptions(),
                         riddle->getCorrectAnswer());
    }
}

// Heart spawn system - spawn at random empty position
void GameLevel::spawnHeart() {
    int attempts = 0;
    int x, y;

    do {
        x = 2 + rand() % (GameConfig::GAME_WIDTH - 4);
        y = 2 + rand() % (GameConfig::GAME_HEIGHT - 4);
        attempts++;
    } while (!isValidHeartPosition(x, y) && attempts < 100);

    if (attempts < 100) {
        heart.setPosition(x, y);
        heart.resetTimer();
        heart.activate();
        heartActive = true;
    }
}

void GameLevel::despawnHeart() {
    if (heartActive) {
        if (!silentMode) {
            gotoxy(heart.getX(), heart.getY());
            std::cout << ' ';
        }
        heartActive = false;
        heartRespawnTimer = HEART_RESPAWN_DELAY;
    }
}

// Update heart timer and handle respawn
void GameLevel::updateHeart() {
    if (heartActive) {
        if (heart.updateTimer()) despawnHeart();
    } else {
        if (heartRespawnTimer > 0) heartRespawnTimer--;
        else spawnHeart();
    }
}

bool GameLevel::isValidHeartPosition(int x, int y) const {
    char cell = board.getCell(x, y);
    if (cell != ' ' && cell != GameConfig::CHAR_EMPTY) return false;

    if (player1 && player1->getPosition().getX() == x && player1->getPosition().getY() == y) return false;
    if (player2 && player2->getPosition().getX() == x && player2->getPosition().getY() == y) return false;

    return true;
}

// AI-written: Bomb countdown and chain reaction explosion system
// Detects bombs in explosion radius, accelerates their timers, preserves them
// during initial blast, then activates chain reactions with reduced fuse time.
// Uses position tracking sets to prevent double-activation.
void GameLevel::updateBombs() {
    int displayCountdown = 0;
    std::vector<Point> chainReactionPositions;
    std::set<Point> activatedPositions;

    for (auto& bomb : objects.getBombs()) {
        if (bomb->isActiveCountdown()) {
            int ticks = bomb->getCountdown();
            if (ticks > 0 && (displayCountdown == 0 || ticks < displayCountdown)) {
                displayCountdown = ticks;
            }

            if (bomb->updateCountdown()) {
                Point explosionOrigin = bomb->getPosition();
                spawnShrapnel(explosionOrigin);

                std::vector<Point> affected = bomb->getExplosionArea(&board);

                // Build list of positions in explosion radius
                std::vector<Point> chainReactionPositionsToCheck;
                int radius = bomb->getExplosionRadius();
                Point bombPos = bomb->getPosition();
                for (int dy = -radius; dy <= radius; dy++) {
                    for (int dx = -radius; dx <= radius; dx++) {
                        if (dx == 0 && dy == 0) continue;
                        int tx = bombPos.getX() + dx;
                        int ty = bombPos.getY() + dy;
                        if (tx >= 1 && ty >= 1 && tx < GameConfig::GAME_WIDTH - 1 && ty < GameConfig::GAME_HEIGHT - 1) {
                            chainReactionPositionsToCheck.push_back(Point(tx, ty));
                        }
                    }
                }

                // Check for other bombs in blast zone
                std::vector<Point> preservePositionsForThisBomb;
                std::vector<Point> chainReactionForThisBomb;

                for (auto& otherBomb : objects.getBombs()) {
                    if (otherBomb.get() == bomb.get()) continue;
                    
                    Point otherBombPos = otherBomb->getPosition();
                    
                    bool withinRadius = false;
                    for (const Point& checkPos : chainReactionPositionsToCheck) {
                        if (otherBombPos == checkPos) {
                            withinRadius = true;
                            break;
                        }
                    }
                    
                    if (withinRadius && activatedPositions.find(otherBombPos) == activatedPositions.end()) {
                        if (otherBomb->isIdle()) {
                            preservePositionsForThisBomb.push_back(otherBombPos);
                            chainReactionForThisBomb.push_back(otherBombPos);
                            activatedPositions.insert(otherBombPos);
                        } else if (otherBomb->isActivated() && otherBomb->isActiveCountdown() && otherBomb->getCountdown() > 1) {
                            preservePositionsForThisBomb.push_back(otherBombPos);
                            while (otherBomb->getCountdown() > 1) {
                                otherBomb->updateCountdown();
                            }
                            activatedPositions.insert(otherBombPos);
                        }
                    }
                }

                bomb->explode(&board, &objects, player1, player2, affected, preservePositionsForThisBomb);
                needsFullRedraw = true;

                if (!LivesManager::hasLivesRemaining()) {
                    result = ScreenResult::GameOver;
                }

                chainReactionPositions.insert(chainReactionPositions.end(), 
                    chainReactionForThisBomb.begin(), chainReactionForThisBomb.end());
            }
        }
    }
    
    // Activate chain reaction bombs
    if (!chainReactionPositions.empty()) {
        int activatedCount = 0;
        for (const Point& pos : chainReactionPositions) {
            for (auto& bomb : objects.getBombs()) {
                if (bomb->isIdle() && bomb->getPosition() == pos) {
                    bomb->activate(pos);
                    while (bomb->getCountdown() > 3) {
                        bomb->updateCountdown();
                    }
                    activatedCount++;
                    break;
                }
            }
        }
        if (activatedCount > 0) {
            messageDisplay.show("Chain reaction! " + std::to_string(activatedCount) + " bomb(s) activated!");
        }
    }

    statusBar.setBombCountdown(displayCountdown);
}

// Spawn shrapnel particles in 8 directions from explosion
void GameLevel::spawnShrapnel(const Point& origin) {
    shrapnelParticles.clear();

    struct DirSym { int dx; int dy; char sym; };
    DirSym rays[] = {
        {-1,  0, '-'}, { 1,  0, '-'},
        { 0, -1, '|'}, { 0,  1, '|'},
        { 1, -1, '/'}, {-1,  1, '/'},
        {-1, -1, '\\'}, { 1,  1, '\\'},
        { 1,  0, ','}, {-1, 0, ','},
        { 0,  1, ','}, { 0, -1, ','},
    };

    for (const auto& r : rays) {
        ShrapnelParticle p;
        p.pos = origin;
        p.prevPos = origin;
        p.origin = origin;
        p.dx = r.dx;
        p.dy = r.dy;
        p.sym = r.sym;
        p.stepsLeft = SHRAPNEL_MAX_STEPS;
        p.active = true;
        shrapnelParticles.push_back(p);
    }

    board.setCell(origin, GameConfig::CHAR_EMPTY);
}

// AI-written: Shrapnel animation with per-explosion player damage tracking
// Particles move outward destroying objects, damaging players once per explosion
// (tracked via explosionPlayerHits map), respecting visibility in dark rooms.
// Cleans up particles and tracking data when explosions complete.
void GameLevel::updateShrapnel() {
    if (shrapnelParticles.empty()) return;

    auto isVisible = [&](int x, int y) {
        return !lighting.getIsDarkRoom() || lighting.isCellVisible(x, y);
    };

    for (auto& p : shrapnelParticles) {
        if (!p.active) continue;

        renderer.drawCell(p.prevPos.getX(), p.prevPos.getY());

        if (p.stepsLeft <= 0) {
            p.active = false;
            continue;
        }

        int nx = p.pos.getX() + p.dx;
        int ny = p.pos.getY() + p.dy;
        p.prevPos = p.pos;
        p.pos.set(nx, ny);
        p.stepsLeft--;

        if (!p.pos.isInBounds()) {
            p.active = false;
            renderer.drawCell(p.prevPos.getX(), p.prevPos.getY());
            continue;
        }

        char cell = board.getCell(nx, ny);
        bool isBorder = (nx == 0 || ny == 0 || nx == GameConfig::GAME_WIDTH - 1 || ny == GameConfig::GAME_HEIGHT - 1);
        if (isBorder && cell == GameConfig::CHAR_WALL) {
            p.active = false;
            renderer.drawCell(p.prevPos.getX(), p.prevPos.getY());
            renderer.drawCell(nx, ny);
            continue;
        }

        bool isAtOrigin = (p.pos.getX() == p.origin.getX() && p.pos.getY() == p.origin.getY());
        
        // Damage players (once per explosion, tracked via map)
        if (!isAtOrigin) {
            auto hitPlayer = [&](Player* player, const char* label, int playerNum) {
                if (player && player->getPosition() == p.pos) {
                    auto& hitPlayers = explosionPlayerHits[p.origin];
                    if (hitPlayers.find(player) == hitPlayers.end()) {
                        Player::subtractLife();
                        hitPlayers.insert(player);
                        messageDisplay.show(std::string(label) + " hit by explosion!");
                        // Record life lost
                        if (game) {
                            game->getRecorder().recordLifeLost(playerNum);
                        }
                        if (!LivesManager::hasLivesRemaining()) {
                            result = ScreenResult::GameOver;
                        }
                    }
                    return true;
                }
                return false;
            };

            hitPlayer(player1, "Player 1", 1) || hitPlayer(player2, "Player 2", 2);
            if (result == ScreenResult::GameOver) {
                p.active = false;
                continue;
            }
        }

        // Destroy objects (doors are protected)
        bool isDoor = (cell >= '1' && cell <= '9');
        if (cell != GameConfig::CHAR_EMPTY && cell != ' ' && !isDoor) {
            objects.destroyAt(p.pos);
            if (heartActive && heart.getPosition() == p.pos) despawnHeart();
            board.setCell(p.pos, GameConfig::CHAR_EMPTY);
        }
    }

    // Cleanup inactive particles
    bool hadParticles = !shrapnelParticles.empty();
    std::set<Point> completedExplosions;
    
    auto it = shrapnelParticles.begin();
    while (it != shrapnelParticles.end()) {
        if (!it->active) {
            completedExplosions.insert(it->origin);
            renderer.drawCell(it->pos.getX(), it->pos.getY());
            renderer.drawCell(it->prevPos.getX(), it->prevPos.getY());
            it = shrapnelParticles.erase(it);
        } else {
            ++it;
        }
    }
    
    // Clear tracking for finished explosions
    for (const Point& origin : completedExplosions) {
        bool hasActiveParticles = false;
        for (const auto& particle : shrapnelParticles) {
            if (particle.active && particle.origin == origin) {
                hasActiveParticles = true;
                break;
            }
        }
        if (!hasActiveParticles) {
            explosionPlayerHits.erase(origin);
        }
    }
    
    if (hadParticles && shrapnelParticles.empty()) {
        needsFullRedraw = true;
    }
}

// Handle spring compression on entry and release on direction change
void GameLevel::handleSpringInteraction(Player& player, Player* otherPlayer) {
    Point pos = player.getPosition();
    Spring* spring = objects.getSpringAt(pos);

    if (spring) {
        Direction dir = player.getDirection();

        if (!spring->getIsCompressed()) {
            spring->onPlayerEnter(&player, pos);
        }
        else if (spring->getCompressingPlayer() == &player) {
            if (spring->shouldRelease(dir)) {
                Spring::ReleaseInfo info = spring->release();
                if (info.released) {
                    player.setSpringMomentum(info.direction, info.speed, info.duration);
                    messageDisplay.show("Spring launched! Speed: " + std::to_string(info.speed));
                }
            } else {
                spring->onPlayerMove(&player, pos, dir);
            }
        }

        spring->updateBoard(&board);
        needsFullRedraw = true;
    }
    else {
        // Exited spring area - release if still compressing
        for (auto& spr : objects.getSprings()) {
            if (spr->getCompressingPlayer() == &player) {
                Spring::ReleaseInfo info = spr->release();
                if (info.released) {
                    player.setSpringMomentum(info.direction, info.speed, info.duration);
                }
                spr->onPlayerExit(&player);
                spr->updateBoard(&board);
                needsFullRedraw = true;
            }
        }
    }
}

// AI-assisted: Cooperative obstacle pushing with combined force calculation
// Checks if other player is adjacent or facing same direction within grace period
// to combine pushing force. Spring momentum adds extra force.
bool GameLevel::tryPushObstacle(Player& player, Player* otherPlayer, Direction dir) {
    Point nextPos = player.getPosition().getNextPosition(dir);
    Obstacle* obstacle = objects.getObstacleAt(nextPos);

    if (!obstacle) return false;

    int force = player.getForce();

    // Check if other player can help push
    if (otherPlayer) {
        Point otherPos = otherPlayer->getPosition();
        int dx = std::abs(player.getPosition().getX() - otherPos.getX());
        int dy = std::abs(player.getPosition().getY() - otherPos.getY());

        bool playersAdjacent = (dx == 1 && dy == 0) || (dx == 0 && dy == 1);
        Point otherNextPos = otherPos.getNextPosition(dir);
        bool otherAdjacentToObstacle = obstacle->containsPoint(otherNextPos);
        
        if (playersAdjacent || otherAdjacentToObstacle) {
            // 5-cycle grace period for coordination
            bool sameDirection = (otherPlayer->getDirection() == dir);
            bool recentlySameDirection = (otherPlayer->getLastMoveDirection() == dir && 
                                          otherPlayer->getLastMoveAge() <= 5);
            
            if (sameDirection || recentlySameDirection) {
                force += otherPlayer->getForce();
            }
        }
    }

    if (obstacle->canPush(dir, &board, force)) {
        obstacle->push(dir, &board, force);
        return true;
    }

    return false;
}
