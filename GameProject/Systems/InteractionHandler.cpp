// InteractionHandler.cpp - Handles player pickups and object interactions
// Processes item collection, switch toggling, riddle answering, and item drops
// AI usage: Input buffer flush before riddle (AI-assisted)

#include "InteractionHandler.h"
#include "GameRecorder.h"
#include "LivesManager.h"
#include "GameConfig.h"
#include "console.h"
#include <iostream>
#include <cctype>
#include <fstream>

InteractionHandler::InteractionHandler()
    : objects(nullptr), board(nullptr), lighting(nullptr),
    statusBar(nullptr), messageDisplay(nullptr), game(nullptr),
    recorder(nullptr), lastTriggeredRiddle(nullptr), lastRiddlePos(-1, -1),
    lastRiddleAnswer('\0') {
}

// Dependency injection
void InteractionHandler::setObjectManager(ObjectManager* obj) { objects = obj; }
void InteractionHandler::setBoard(GameBoard* b) { board = b; }
void InteractionHandler::setLightingSystem(LightingSystem* light) { lighting = light; }
void InteractionHandler::setStatusBar(StatusBar* status) { statusBar = status; }
void InteractionHandler::setMessageDisplay(MessageDisplay* msg) { messageDisplay = msg; }
void InteractionHandler::setGame(Game* g) { game = g; }
void InteractionHandler::setRecorder(GameRecorder* rec) { recorder = rec; }

// Process all possible interactions at player position
InteractionResult InteractionHandler::handleInteractions(Player& player) {
    InteractionResult result;
    if (!objects || !board) return result;

    Point pos = player.getPosition();

    // Light switch toggles room lighting
    LightSwitch* lightSw = objects->getLightSwitchAt(pos);
    if (lightSw && lightSw->isActive()) {
        lightSw->toggle();
        board->setCell(pos, lightSw->getSymbol());
        result.needsFullRedraw = true;
        if (messageDisplay) {
            messageDisplay->show(lightSw->getIsOn() ? "Light ON!" : "Light OFF!");
        }
        return result;
    }

    // Gate-linked switch may open/close gate
    Switch* sw = objects->getSwitchAt(pos);
    if (sw && sw->isActive()) {
        sw->toggle();
        board->setCell(pos, sw->getSymbol());
        result.needsFullRedraw = true;

        char linkedGate = sw->getLinkedGate();

        if (objects->areAllSwitchesOnForGate(linkedGate)) {
            Gate* gate = objects->getGateByDoorNum(linkedGate);
            if (gate && !gate->getIsOpen()) {
                openGate(*gate);
                if (messageDisplay) {
                    messageDisplay->show("All switches ON - Gate opened!");
                }
            }
        }
        else {
            Gate* gate = objects->getGateByDoorNum(linkedGate);
            if (gate && gate->getIsOpen()) {
                closeGate(*gate);
                if (messageDisplay) {
                    messageDisplay->show("Switch OFF - Gate locked!");
                }
            }
            else {
                if (messageDisplay) {
                    messageDisplay->show(sw->getIsOn() ? "Switch ON!" : "Switch OFF!");
                }
            }
        }
        return result;
    }

    // Key pickup
    Key* key = objects->getKeyAt(pos);
    if (key && key->isActive() && !player.hasItem()) {
        key->onInteract(player);
        board->setCell(pos, GameConfig::CHAR_EMPTY);
        result.needsRedraw = true;
        if (statusBar && lighting) {
            statusBar->setDarkRoomStatus(lighting->getIsDarkRoom(), lighting->isRoomLit());
            statusBar->draw();
        }
        if (messageDisplay) {
            messageDisplay->show("Picked up a KEY!");
        }
        return result;
    }

    // Coins can be collected even while holding items
    Coin* coin = objects->getCoinAt(pos);
    if (coin && coin->isActive()) {
        coin->onInteract(player);
        board->setCell(pos, GameConfig::CHAR_EMPTY);
        result.needsRedraw = true;
        if (statusBar && lighting) {
            statusBar->setDarkRoomStatus(lighting->getIsDarkRoom(), lighting->isRoomLit());
            statusBar->draw();
        }
        if (messageDisplay) {
            messageDisplay->show("Picked up a COIN!");
        }
        return result;
    }

    // Dropped key pickup
    ObjectManager::DroppedKeyItem* droppedKey = objects->getDroppedKeyAt(pos);
    if (droppedKey && !player.hasItem()) {
        droppedKey->collected = true;
        player.pickupItem(GameConfig::CHAR_KEY);
        board->setCell(pos, GameConfig::CHAR_EMPTY);
        result.needsRedraw = true;
        if (statusBar && lighting) {
            statusBar->setDarkRoomStatus(lighting->getIsDarkRoom(), lighting->isRoomLit());
            statusBar->draw();
        }
        if (messageDisplay) {
            messageDisplay->show("Picked up a KEY!");
        }
        return result;
    }

    // Torch pickup
    Torch* torch = objects->getTorchAt(pos);
    if (torch && torch->isActive() && !player.hasItem()) {
        torch->onInteract(player);
        board->setCell(pos, GameConfig::CHAR_EMPTY);
        result.needsFullRedraw = true;
        if (statusBar && lighting) {
            statusBar->setDarkRoomStatus(lighting->getIsDarkRoom(), lighting->isRoomLit());
            statusBar->draw();
        }
        if (messageDisplay) {
            messageDisplay->show("Picked up a TORCH!");
        }
        return result;
    }

    // Dropped torch pickup
    DroppedTorch* droppedTorch = objects->getDroppedTorchAt(pos);
    if (droppedTorch && !player.hasItem()) {
        droppedTorch->collected = true;
        player.pickupItem(GameConfig::CHAR_TORCH);
        board->setCell(pos, GameConfig::CHAR_EMPTY);
        result.needsFullRedraw = true;
        if (statusBar && lighting) {
            statusBar->setDarkRoomStatus(lighting->getIsDarkRoom(), lighting->isRoomLit());
            statusBar->draw();
        }
        if (messageDisplay) {
            messageDisplay->show("Picked up a TORCH!");
        }
        return result;
    }

    // Bomb pickup
    Bomb* bomb = objects->getBombAt(pos);
    if (bomb && bomb->isIdle() && !player.hasItem()) {
        bomb->collect();
        player.pickupItem('B');
        board->setCell(pos, GameConfig::CHAR_EMPTY);
        result.needsRedraw = true;
        if (statusBar && lighting) {
            statusBar->setDarkRoomStatus(lighting->getIsDarkRoom(), lighting->isRoomLit());
            statusBar->draw();
        }
        if (messageDisplay) {
            messageDisplay->show("Picked up a BOMB! Press drop key to plant.");
        }
        return result;
    }

    return result;
}

// Attempt to open gate using player's key
bool InteractionHandler::tryOpenGate(Player& player, const Point& nextPos) {
    if (!objects) return false;

    Gate* gate = objects->getGateAt(nextPos);
    if (gate && !gate->getIsOpen()) {
        if (player.hasKey()) {
            player.disposeItem();
            openGate(*gate);
            if (statusBar && lighting) {
                statusBar->setDarkRoomStatus(lighting->getIsDarkRoom(), lighting->isRoomLit());
                statusBar->draw();
            }
            if (messageDisplay) {
                messageDisplay->show("Gate opened!");
            }
            return true;
        }
        else {
            if (messageDisplay) {
                messageDisplay->show("You need a KEY to open this gate!");
            }
        }
    }
    return false;
}

// Drop held item (key, torch, or plant bomb)
void InteractionHandler::dropPlayerItem(Player& player) {
    if (!objects) return;

    if (player.hasKey()) {
        player.disposeItem();
        objects->dropKey(player.getPosition());
        if (statusBar && lighting) {
            statusBar->setDarkRoomStatus(lighting->getIsDarkRoom(), lighting->isRoomLit());
            statusBar->draw();
        }
        if (messageDisplay) {
            messageDisplay->show("Dropped a KEY!");
        }
    }
    else if (player.hasTorch()) {
        player.disposeItem();
        int radius = lighting ? lighting->getTorchRadius() : 5;
        objects->dropTorch(player.getPosition(), radius);
        if (statusBar && lighting) {
            statusBar->setDarkRoomStatus(lighting->getIsDarkRoom(), lighting->isRoomLit());
            statusBar->draw();
        }
        if (messageDisplay) {
            messageDisplay->show("Dropped a TORCH!");
        }
    }
    else if (player.hasBomb()) {
        player.disposeItem();
        // Find the collected bomb and activate it
        for (auto& bomb : objects->getBombs()) {
            if (bomb->isCollected()) {
                bomb->activate(player.getPosition());
                if (board) {
                    board->setCell(player.getPosition(), GameConfig::CHAR_EMPTY);
                }
                break;
            }
        }
        if (statusBar && lighting) {
            statusBar->setDarkRoomStatus(lighting->getIsDarkRoom(), lighting->isRoomLit());
            statusBar->draw();
        }
        if (messageDisplay) {
            messageDisplay->show("BOMB PLANTED! 5 cycles to explosion!");
        }
    }
}

void InteractionHandler::openGate(Gate& gate) {
    if (!board) return;
    gate.open();
    gate.clearBarsFromBoard(board->getRawBoard());
}

void InteractionHandler::closeGate(Gate& gate) {
    if (!board) return;
    gate.close();
    gate.updateBoard(board->getRawBoard());
}

// Display riddle question and answer options
void InteractionHandler::displayRiddleUI(const Riddle& riddle) {
    // #region agent log H4: Log which riddle is being displayed
    { std::ofstream dbg("c:\\Users\\admin\\source\\repos\\GameProject\\Exe_2\\.cursor\\debug.log", std::ios::app); std::string q = riddle.getQuestion(); dbg << "{\"location\":\"InteractionHandler.cpp:displayRiddleUI\",\"message\":\"Displaying riddle\",\"data\":{\"question\":\"" << q.substr(0, 60) << "\"},\"hypothesisId\":\"H4\",\"timestamp\":" << time(nullptr) << "}\n"; }
    // #endregion
    int startY = GameConfig::GAME_HEIGHT + 1;

    for (int i = 0; i < 7; i++) {
        gotoxy(0, startY + i);
        std::cout << std::string(GameConfig::GAME_WIDTH, ' ');
    }

    gotoxy(0, startY);
    setColor(Color::LightYellow);
    std::cout << "RIDDLE: " << riddle.getQuestion();

    const auto& options = riddle.getOptions();
    char optionChar = 'a';
    for (size_t i = 0; i < options.size() && i < 4; i++) {
        gotoxy(2, startY + 2 + (int)i);
        setColor(Color::White);
        std::cout << optionChar << ") " << options[i];
        optionChar++;
    }

    gotoxy(0, startY + 6);
    setColor(Color::LightCyan);
    std::cout << "Press a, b, c, or d to answer: ";
    resetColor();
    std::cout.flush();
}

void InteractionHandler::clearRiddleUI() {
    int startY = GameConfig::GAME_HEIGHT + 1;
    for (int i = 0; i < 7; i++) {
        gotoxy(0, startY + i);
        std::cout << std::string(GameConfig::GAME_WIDTH, ' ');
    }
    std::cout.flush();
}

// AI-assisted: Wait for riddle answer with input buffer flush
// In load mode, retrieves answer from recorded steps instead
char InteractionHandler::waitForRiddleAnswer(int playerNum) {
    // In load mode, get the answer from recorded steps
    if (recorder && recorder->isLoadMode()) {
        char recordedAnswer = recorder->getRiddleAnswer(playerNum);
        if (recordedAnswer != '\0') {
            return recordedAnswer;
        }
        // Fallback if no recorded answer found
        return 'a';
    }
    
    // Normal/save mode: wait for keyboard input
    while (_kbhit()) _getch();

    while (true) {
        if (_kbhit()) {
            char ch = (char)_getch();
            ch = static_cast<char>(std::tolower(ch));
            if (ch == 'a' || ch == 'b' || ch == 'c' || ch == 'd') {
                return ch;
            }
        }
        Sleep(10);
    }
}

// Display riddle, get answer, check correctness, update lives
bool InteractionHandler::handleRiddle(Player& player, Riddle& riddle, int playerNum) {
    // In silent mode, skip UI entirely
    bool isSilent = recorder && recorder->isSilentMode();
    
    if (!isSilent) {
        displayRiddleUI(riddle);
    }

    char answer = waitForRiddleAnswer(playerNum);
    lastRiddleAnswer = answer;  // Store for recording
    
    // Record the answer in save mode
    if (recorder && recorder->isSaveMode()) {
        recorder->recordRiddleAnswer(playerNum, answer);
    }
    
    bool correct = riddle.checkAnswer(answer);

    int startY = GameConfig::GAME_HEIGHT + 1;

    if (correct) {
        if (!isSilent) {
            gotoxy(0, startY + 6);
            setColor(Color::LightGreen);
            std::cout << "CORRECT! Well done!                    ";
            resetColor();
            std::cout.flush();
            Sleep(1500);
        }
    }
    else {
        LivesManager::subtractLife();

        if (!isSilent) {
            gotoxy(0, startY + 6);
            setColor(Color::LightRed);
            std::cout << "WRONG! Lives remaining: " << LivesManager::getLives() << "        ";
            resetColor();
            std::cout.flush();
            Sleep(2000);
        }
    }

    if (!isSilent) {
        clearRiddleUI();
    }

    return correct;
}
