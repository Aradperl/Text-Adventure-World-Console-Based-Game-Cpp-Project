// InputHandler.cpp - Keyboard input processing for both players
// Handles live input in normal/save mode, replays from steps in load mode

#include "InputHandler.h"
#include <conio.h>
#include <cctype>

// Constructor
InputHandler::InputHandler() 
    : player1(nullptr), player2(nullptr), isPaused(false), recorder(nullptr) {}

// Setup players
void InputHandler::setPlayers(Player* p1, Player* p2) {
    player1 = p1;
    player2 = p2;
}

// Setup recorder
void InputHandler::setRecorder(GameRecorder* rec) {
    recorder = rec;
}

// Pause state
void InputHandler::setPaused(bool paused) {
    isPaused = paused;
}

bool InputHandler::getIsPaused() const {
    return isPaused;
}

// Handle a single key press
void InputHandler::handleKey(char key, InputResult& result) {
    key = std::toupper(key);
    
    // ESC key - but ignore in load mode
    if (key == GameConfig::KEY_ESC) {
        if (!recorder || !recorder->isLoadMode()) {
            result.escPressed = true;
        }
        return;
    }
    
    // When paused, check for menu or save
    if (isPaused) {
        if (key == 'H') {
            result.menuRequested = true;
        } else if (key == 'S') {
            result.saveRequested = true;
        }
        return;
    }
    
    // Track direction changes for recording
    Direction p1OldDir = player1 ? player1->getDirection() : Direction::STAY;
    Direction p2OldDir = player2 ? player2->getDirection() : Direction::STAY;
    
    // Player 1 controls
    if (player1) {
        switch (key) {
            case GameConfig::P1_UP:
                player1->setDirection(Direction::UP);
                if (recorder && recorder->isSaveMode() && Direction::UP != p1OldDir)
                    recorder->recordStep(1, Direction::UP);
                break;
            case GameConfig::P1_DOWN:
                player1->setDirection(Direction::DOWN);
                if (recorder && recorder->isSaveMode() && Direction::DOWN != p1OldDir)
                    recorder->recordStep(1, Direction::DOWN);
                break;
            case GameConfig::P1_LEFT:
                player1->setDirection(Direction::LEFT);
                if (recorder && recorder->isSaveMode() && Direction::LEFT != p1OldDir)
                    recorder->recordStep(1, Direction::LEFT);
                break;
            case GameConfig::P1_RIGHT:
                player1->setDirection(Direction::RIGHT);
                if (recorder && recorder->isSaveMode() && Direction::RIGHT != p1OldDir)
                    recorder->recordStep(1, Direction::RIGHT);
                break;
            case GameConfig::P1_STAY:
                player1->stop();
                if (recorder && recorder->isSaveMode())
                    recorder->recordStop(1);
                break;
            case GameConfig::P1_DISPOSE:
                result.player1Dispose = true;
                if (recorder && recorder->isSaveMode())
                    recorder->recordDispose(1);
                break;
        }
    }
    
    // Player 2 controls
    if (player2) {
        switch (key) {
            case GameConfig::P2_UP:
                player2->setDirection(Direction::UP);
                if (recorder && recorder->isSaveMode() && Direction::UP != p2OldDir)
                    recorder->recordStep(2, Direction::UP);
                break;
            case GameConfig::P2_DOWN:
                player2->setDirection(Direction::DOWN);
                if (recorder && recorder->isSaveMode() && Direction::DOWN != p2OldDir)
                    recorder->recordStep(2, Direction::DOWN);
                break;
            case GameConfig::P2_LEFT:
                player2->setDirection(Direction::LEFT);
                if (recorder && recorder->isSaveMode() && Direction::LEFT != p2OldDir)
                    recorder->recordStep(2, Direction::LEFT);
                break;
            case GameConfig::P2_RIGHT:
                player2->setDirection(Direction::RIGHT);
                if (recorder && recorder->isSaveMode() && Direction::RIGHT != p2OldDir)
                    recorder->recordStep(2, Direction::RIGHT);
                break;
            case GameConfig::P2_STAY:
                player2->stop();
                if (recorder && recorder->isSaveMode())
                    recorder->recordStop(2);
                break;
            case GameConfig::P2_DISPOSE:
                result.player2Dispose = true;
                if (recorder && recorder->isSaveMode())
                    recorder->recordDispose(2);
                break;
        }
    }
}

// Get recorded input for a specific player
char InputHandler::getRecordedInput(int player) {
    if (!recorder || !recorder->isLoadMode()) {
        return '\0';
    }
    
    // Look at steps for current cycle
    while (recorder->hasNextStep()) {
        GameStep step = recorder->peekNextStep();
        
        // If step is for a future cycle, stop
        if (step.cycle > recorder->getCurrentCycle()) {
            break;
        }
        
        // If step is for current cycle
        if (step.cycle == recorder->getCurrentCycle()) {
            if (step.player == player) {
                // Consume the step
                recorder->getNextStep();
                return step.action;
            } else {
                // Step is for other player, keep looking
                // But don't skip it - it will be consumed by the other player's call
                break;
            }
        }
        
        // Skip past steps (shouldn't happen normally)
        recorder->getNextStep();
    }
    
    return '\0';
}

// Process keyboard input
InputResult InputHandler::processInput() {
    InputResult result;
    
    // In load mode, read from recorded steps instead of keyboard
    // Note: Riddle answers are stored separately and handled by getRiddleAnswer()
    if (recorder && recorder->isLoadMode()) {
        // Process all movement steps for current cycle
        while (recorder->hasNextStep()) {
            GameStep step = recorder->peekNextStep();
            
            // Stop if this step is for a future cycle
            if (step.cycle > recorder->getCurrentCycle()) {
                break;
            }
            
            // Process this step
            step = recorder->getNextStep();
            
            // Apply the action
            if (step.player == 1 && player1) {
                switch (std::toupper(step.action)) {
                    case 'U': player1->setDirection(Direction::UP); break;
                    case 'D': player1->setDirection(Direction::DOWN); break;
                    case 'L': player1->setDirection(Direction::LEFT); break;
                    case 'R': player1->setDirection(Direction::RIGHT); break;
                    case 'S': player1->stop(); break;
                    case 'E': result.player1Dispose = true; break;
                }
            } else if (step.player == 2 && player2) {
                switch (std::toupper(step.action)) {
                    case 'U': player2->setDirection(Direction::UP); break;
                    case 'D': player2->setDirection(Direction::DOWN); break;
                    case 'L': player2->setDirection(Direction::LEFT); break;
                    case 'R': player2->setDirection(Direction::RIGHT); break;
                    case 'S': player2->stop(); break;
                    case 'O': result.player2Dispose = true; break;
                }
            }
        }
        
        return result;
    }
    
    // Normal mode: read from keyboard
    if (!_kbhit()) {
        return result;
    }
    
    char key = (char)_getch();
    handleKey(key, result);
    
    return result;
}
