// Game.h - Main game class

#pragma once

#include "console.h"
#include "GameConfig.h"
#include "Player.h"
#include "BaseScreen.h"
#include "MenuScreen.h"
#include "InstructionsScreen.h"
#include "EndScreen.h"
#include "LoadGameScreen.h"
#include "GameLevel.h"
#include "GameRecorder.h"
#include "GameStateSaver.h"
#include <vector>
#include <memory>
#include <set>
#include <string>

// Global riddle tracking
inline std::set<std::string>& getUsedRiddles() {
    static std::set<std::string> usedRiddles;
    return usedRiddles;
}

inline bool isRiddleUsed(const std::string& question) {
    return getUsedRiddles().find(question) != getUsedRiddles().end();
}
inline void markRiddleUsed(const std::string& question) {
    getUsedRiddles().insert(question);
}
inline void resetUsedRiddles() {
    getUsedRiddles().clear();
}

// Game states
enum class GameState {
    Menu,
    Instructions,
    LoadSave,
    Playing,
    Victory,
    Exit
};

class Game {
private:
    // Players (shared with levels)
    Player player1;
    Player player2;
    
    // Player start positions (loaded from screen files)
    int player1StartX, player1StartY;
    int player2StartX, player2StartY;
    
    // Screens
    std::unique_ptr<MenuScreen> menuScreen;
    std::unique_ptr<InstructionsScreen> instructionsScreen;
    std::unique_ptr<EndScreen> endScreen;
    std::unique_ptr<LoadGameScreen> loadGameScreen;
    std::vector<std::unique_ptr<GameLevel>> gameLevels;
    
    // Current state
    GameState currentState;
    int currentLevelIndex;
    bool isRunning;
    bool noScreenFilesFound;
    
    // Recording/playback system
    GameRecorder recorder;
    
public:
    // Getters
    GameState getState() const { return currentState; }
    int getCurrentLevel() const { return currentLevelIndex; }
    
    // Recorder access
    GameRecorder& getRecorder() { return recorder; }
    const GameRecorder& getRecorder() const { return recorder; }

private:
    // Setup and level creation
    void initScreens();
    void initGameLevels();
    void createVictoryLevel();
    void handleScreenResult(ScreenResult result);
    void resetPlayers();
    void runCurrentScreen();
    
    // Mode-specific run methods
    void runNormalMode();
    void runSaveMode();
    void runLoadMode();

public:
    Game();
    Game(GameMode mode);
    void run();
    
    // Save/Load game state
    bool saveCurrentState();
    bool loadFromSaveFile(const std::string& filename);
    
    // Player access for GameLevel save functionality
    Player& getPlayer1() { return player1; }
    Player& getPlayer2() { return player2; }
    const Player& getPlayer1() const { return player1; }
    const Player& getPlayer2() const { return player2; }
};
