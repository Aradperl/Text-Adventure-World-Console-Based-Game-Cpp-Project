// Game.cpp - Main game loop and state management
// Handles game states, level transitions, save/load, and recording modes
// AI usage: Level flow control and dynamic screen ordering (AI-assisted)

#include "Game.h"
#include "LivesManager.h"
#include "ScreenParser.h"
#include "StatusBar.h"
#include "MessageDisplay.h"
#include <cctype>
#include <iostream>

// Default constructor for normal gameplay mode
Game::Game()
    : player1(15, 5, GameConfig::CHAR_PLAYER1, Color::LightGreen),
    player2(20, 5, GameConfig::CHAR_PLAYER2, Color::LightCyan),
    player1StartX(2), player1StartY(2),
    player2StartX(2), player2StartY(18),
    currentState(GameState::Menu),
    currentLevelIndex(0),
    isRunning(true),
    noScreenFilesFound(false) {
    recorder.setMode(GameMode::Normal);
    ScreenParser::setRecorder(&recorder);
    initScreens();
}

// Constructor with recording/playback mode
Game::Game(GameMode mode)
    : player1(15, 5, GameConfig::CHAR_PLAYER1, Color::LightGreen),
    player2(20, 5, GameConfig::CHAR_PLAYER2, Color::LightCyan),
    player1StartX(2), player1StartY(2),
    player2StartX(2), player2StartY(18),
    currentState(GameState::Menu),
    currentLevelIndex(0),
    isRunning(true),
    noScreenFilesFound(false) {
    recorder.setMode(mode);
    
    // Set silent mode globally for rendering classes
    Player::setSilentMode(recorder.isSilentMode());
    StatusBar::setSilentMode(recorder.isSilentMode());
    MessageDisplay::setSilentMode(recorder.isSilentMode());
    
    // Set the recorder in ScreenParser so it can access riddle questions
    ScreenParser::setRecorder(&recorder);
    
    // In load mode, load the steps file FIRST to get the random seed and riddle questions
    // This ensures riddles are selected the same way as during recording
    if (mode == GameMode::Load || mode == GameMode::LoadSilent) {
        if (recorder.loadStepsFile()) {
            // Apply the seed before loading levels
            recorder.setRandomSeed(recorder.getRandomSeed());
        }
    }
    
    initScreens();
}

// Initialize all menu screens and load game levels
void Game::initScreens() {
    menuScreen = std::make_unique<MenuScreen>();
    instructionsScreen = std::make_unique<InstructionsScreen>();
    endScreen = std::make_unique<EndScreen>();
    loadGameScreen = std::make_unique<LoadGameScreen>();
    initGameLevels();
}

// AI-assisted: Dynamic level discovery and loading from screen files
// Scans for adv-world_XX.screen files, parses them into playable levels,
// and extracts player start positions from the first level.
void Game::initGameLevels() {
    ScreenParser parser;

    // Find all adv-world_XX.screen files in sorted order
    std::vector<std::string> screenFiles = ScreenParser::discoverScreenFiles();

    if (screenFiles.empty()) {
        noScreenFilesFound = true;
        return;
    }

    noScreenFilesFound = false;
    
    // Store screen files in recorder
    recorder.setScreenFiles(screenFiles);

    // Parse each file into a playable level
    for (const std::string& filename : screenFiles) {
        auto level = parser.loadFromFile(filename, getUsedRiddles());

        if (level) {
            level->setPlayers(&player1, &player2);
            level->setGame(this);
            level->setSilentMode(recorder.isSilentMode());
            level->init();
            gameLevels.push_back(std::move(level));

            // Grab start positions from first level
            if (gameLevels.size() == 1) {
                int x, y;
                if (parser.getPlayer1Start(x, y)) {
                    player1StartX = x;
                    player1StartY = y;
                }
                if (parser.getPlayer2Start(x, y)) {
                    player2StartX = x;
                    player2StartY = y;
                }
            }
        }
        else {
            std::cerr << "Failed to load " << filename << ": " << parser.getError() << std::endl;
        }
    }

    createVictoryLevel();
}

// Create placeholder victory level
void Game::createVictoryLevel() {
    auto level = std::make_unique<GameLevel>(3);
    level->setPlayers(&player1, &player2);
    level->setSilentMode(recorder.isSilentMode());
    level->init();
    gameLevels.push_back(std::move(level));
}

// Reset players to starting positions and clear inventory
void Game::resetPlayers() {
    player1.setPosition(player1StartX, player1StartY);
    player1.stop();
    player1.disposeItem();

    player2.setPosition(player2StartX, player2StartY);
    player2.stop();
    player2.disposeItem();
}

// AI-assisted: Handle screen transitions and game flow
// Processes level completion, door targets for non-linear level order,
// game over, victory, and menu navigation. Door target system allows
// levels to specify which screen to transition to.
void Game::handleScreenResult(ScreenResult result) {
    switch (result) {

    case ScreenResult::StartGame:
        // Show error if no levels found
        if (noScreenFilesFound || gameLevels.empty()) {
            if (!recorder.isSilentMode()) {
                clrscr();
                gotoxy(20, 10);
                setColor(Color::LightRed);
                std::cout << "ERROR: No screen files found!";
                gotoxy(15, 12);
                setColor(Color::White);
                std::cout << "Please add adv-world_XX.screen files";
                gotoxy(15, 13);
                std::cout << "to the working directory.";
                gotoxy(20, 15);
                setColor(Color::Gray);
                std::cout << "Press any key to return...";
                resetColor();
                std::cout.flush();
                while (!_kbhit()) Sleep(10);
                _getch();
            }
            currentState = GameState::Menu;
            break;
        }

        // Fresh start - reload everything
        gameLevels.clear();
        ScreenParser::resetRiddleIndex();  // Reset riddle counter for new game
        initGameLevels();
        ::resetUsedRiddles();
        
        currentState = GameState::Playing;
        currentLevelIndex = 0;
        LivesManager::resetLives();
        LivesManager::resetCoins();
        resetPlayers();
        recorder.resetCycle();
        
        if (!recorder.isSilentMode()) {
            clrscr();
        }
        break;

    case ScreenResult::ShowInstructions:
        currentState = GameState::Instructions;
        break;

    case ScreenResult::NextScreen: {
        // Get the door's target screen from the current level
        int targetScreen = gameLevels[currentLevelIndex]->getDoorTargetScreen();
        
        // Target is 1-based (target: 1 = adv-world_01.screen), convert to 0-based index
        if (targetScreen >= 1 && targetScreen <= (int)gameLevels.size()) {
            currentLevelIndex = targetScreen - 1;  // Convert 1-based to 0-based
        } else {
            // Default: go to next screen sequentially
            currentLevelIndex++;
        }
        
        // Record screen transition (use level index + 1 for 1-based screen number)
        recorder.recordScreenChange(0, currentLevelIndex + 1);

        // Check if we beat all levels
        if (currentLevelIndex >= (int)gameLevels.size() - 1) {
            currentState = GameState::Victory;
            recorder.recordGameEnd(LivesManager::getCoins(), true);
        }
        else {
            // Reset the target level (restores objects, doors, etc.)
            gameLevels[currentLevelIndex]->reset();
            resetPlayers();
            // Note: clrscr() is called in enter(), no need to call it here
        }
        break;
    }

    case ScreenResult::ShowLoadScreen:
        currentState = GameState::LoadSave;
        break;

    case ScreenResult::LoadGame: {
        // Get selected filename from load screen and load the game
        std::string filename = loadGameScreen->getSelectedFilename();
        if (!filename.empty() && loadFromSaveFile(filename)) {
            currentState = GameState::Playing;
            if (!recorder.isSilentMode()) {
                clrscr();
            }
        } else {
            // Load failed, return to menu
            currentState = GameState::Menu;
        }
        break;
    }

    case ScreenResult::ExitToMenu:
        currentState = GameState::Menu;
        break;

    case ScreenResult::ExitGame:
        currentState = GameState::Exit;
        isRunning = false;
        break;

    case ScreenResult::GameOver:
        recorder.recordGameEnd(LivesManager::getCoins(), false);
        
        if (!recorder.isSilentMode()) {
            clrscr();
            gotoxy(30, 10);
            setColor(Color::LightRed);
            std::cout << "GAME OVER!";
            gotoxy(25, 12);
            setColor(Color::White);
            std::cout << "You ran out of lives!";
            gotoxy(22, 14);
            setColor(Color::Gray);
            std::cout << "Press any key to continue...";
            resetColor();
            std::cout.flush();
            
            if (!recorder.isLoadMode()) {
                while (!_kbhit()) Sleep(10);
                _getch();
            } else {
                Sleep(500);  // Brief pause in load mode
            }
        }
        
        // In load mode, just exit after game over
        if (recorder.isLoadMode()) {
            isRunning = false;
        } else {
            currentState = GameState::Menu;
        }
        break;

    case ScreenResult::None:
    default:
        break;
    }
}

// Run game loop for current active screen based on state
void Game::runCurrentScreen() {
    BaseScreen* currentScreen = nullptr;

    switch (currentState) {
    case GameState::Menu:
        currentScreen = menuScreen.get();
        break;
    case GameState::Instructions:
        currentScreen = instructionsScreen.get();
        break;
    case GameState::LoadSave:
        currentScreen = loadGameScreen.get();
        break;
    case GameState::Playing:
        if (currentLevelIndex >= 0 && currentLevelIndex < (int)gameLevels.size()) {
            currentScreen = gameLevels[currentLevelIndex].get();
        }
        break;
    case GameState::Victory:
        currentScreen = endScreen.get();
        break;
    case GameState::Exit:
        return;
    }

    if (currentScreen) {
        // First time entering this screen
        if (!currentScreen->isActive()) {
            currentScreen->enter();
            if (!recorder.isSilentMode()) {
                currentScreen->render();
            }
        }

        // Standard game loop
        currentScreen->handleInput();
        currentScreen->update();
        
        if (!recorder.isSilentMode()) {
            currentScreen->render();
        }

        // Check if screen wants to transition
        ScreenResult result = currentScreen->getResult();
        if (result != ScreenResult::None) {
            currentScreen->exit();
            handleScreenResult(result);
        }
    }
}

// Run interactive gameplay mode
void Game::runNormalMode() {
    hideCursor();

    while (isRunning) {
        runCurrentScreen();

        // Slow down gameplay to playable speed
        if (currentState == GameState::Playing) {
            Sleep(GameConfig::GAME_CYCLE_MS);
        }
    }

    // Goodbye message
    clrscr();
    showCursor();
    gotoxy(0, 0);
    std::cout << "Thanks for playing Text Adventure World!" << std::endl;
}

// Run in recording mode - saves all inputs to steps file
void Game::runSaveMode() {
    hideCursor();
    
    // Initialize random seed and record it
    recorder.initializeRandomSeed();

    while (isRunning) {
        runCurrentScreen();

        // Increment cycle counter when playing
        if (currentState == GameState::Playing) {
            recorder.nextCycle();
            Sleep(GameConfig::GAME_CYCLE_MS);
        }
    }

    // Save files when done
    recorder.saveStepsFile();
    recorder.saveResultFile();
    
    std::cout << "\nGame recording saved to adv-world.steps and adv-world.result\n";

    // Goodbye message
    clrscr();
    showCursor();
    gotoxy(0, 0);
    std::cout << "Thanks for playing Text Adventure World!" << std::endl;
    std::cout << "Game recorded to adv-world.steps and adv-world.result" << std::endl;
}

// Run in playback mode - replays from recorded steps and verifies results
void Game::runLoadMode() {
    // Load files
    if (!recorder.loadStepsFile()) {
        std::cout << "ERROR: Failed to load steps file (adv-world.steps). Exiting.\n";
        std::cout.flush();
        return;
    }
    
    if (!recorder.loadResultFile()) {
        std::cout << "ERROR: Failed to load result file (adv-world.result). Exiting.\n";
        std::cout.flush();
        return;
    }
    
    // Verify screen files match
    const auto& expectedScreens = recorder.getScreenFiles();
    std::vector<std::string> actualScreens = ScreenParser::discoverScreenFiles();
    
    if (expectedScreens.size() != actualScreens.size()) {
        std::cerr << "Warning: Screen file count mismatch!\n";
        std::cerr << "Expected " << expectedScreens.size() << " screens, found " 
                  << actualScreens.size() << "\n";
    } else {
        for (size_t i = 0; i < expectedScreens.size(); i++) {
            if (expectedScreens[i] != actualScreens[i]) {
                std::cerr << "Warning: Screen file mismatch at position " << i << "\n";
                std::cerr << "Expected: " << expectedScreens[i] << "\n";
                std::cerr << "Found: " << actualScreens[i] << "\n";
            }
        }
    }
    
    // Set random seed from file
    recorder.setRandomSeed(recorder.getRandomSeed());
    
    if (!recorder.isSilentMode()) {
        hideCursor();
    }
    
    // In load mode, skip menu and start game directly
    currentState = GameState::Playing;
    currentLevelIndex = 0;
    LivesManager::resetLives();
    LivesManager::resetCoins();
    resetPlayers();
    recorder.resetCycle();  // Ensure cycle counter starts at 0
    
    // Clear and redraw for initial state
    if (!recorder.isSilentMode()) {
        clrscr();
    }

    while (isRunning && currentState == GameState::Playing) {
        runCurrentScreen();
        recorder.nextCycle();

        // In silent mode, run as fast as possible
        // In visual load mode, use shorter delay
        if (!recorder.isSilentMode()) {
            Sleep(GameConfig::GAME_CYCLE_MS / 2);  // Faster playback
        }
        
        // Safety: Exit if all steps processed and game hasn't naturally ended
        // In silent mode, use a shorter buffer since we want fast results
        int safetyBuffer = recorder.isSilentMode() ? 10 : 30;
        if (!recorder.hasNextStep() && recorder.getCurrentCycle() > recorder.getLastStepCycle() + safetyBuffer) {
            break;
        }
    }
    
    // Handle victory state display (event already recorded in handleScreenResult)
    if (currentState == GameState::Victory) {
        if (!recorder.isSilentMode()) {
            endScreen->enter();
            endScreen->render();
            Sleep(2000);  // Show victory for 2 seconds
        }
    }
    
    // Clear screen and restore cursor before printing results
    if (!recorder.isSilentMode()) {
        clrscr();
        showCursor();
        gotoxy(0, 0);
    }
    
    // Verify and report results
    recorder.printVerificationReport();
    
    // Ensure output is flushed and program exits cleanly
    std::cout.flush();
}

// Main entry point - dispatches to appropriate run mode
void Game::run() {
    switch (recorder.getMode()) {
    case GameMode::Normal:
        runNormalMode();
        break;
    case GameMode::Save:
        runSaveMode();
        break;
    case GameMode::Load:
    case GameMode::LoadSilent:
        runLoadMode();
        break;
    }
}

// Save current progress to file for later continuation
bool Game::saveCurrentState() {
    SavedGameState state;
    
    // Current progress
    state.levelIndex = currentLevelIndex;
    state.lives = LivesManager::getLives();
    state.coins = LivesManager::getCoins();
    
    // Player 1 state
    state.player1X = player1.getPosition().getX();
    state.player1Y = player1.getPosition().getY();
    state.player1Item = player1.getHeldItem();
    
    // Player 2 state
    state.player2X = player2.getPosition().getX();
    state.player2Y = player2.getPosition().getY();
    state.player2Item = player2.getHeldItem();
    
    // Note: Level-specific state (collected items, etc.) would require
    // tracking changes in ObjectManager. For now, save basic state.
    // The game will reset the level when loaded, so items are restored.
    
    return GameStateSaver::saveGameState(state);
}

// Load and restore game state from save file
bool Game::loadFromSaveFile(const std::string& filename) {
    SavedGameState state;
    
    if (!GameStateSaver::loadGameState(filename, state)) {
        return false;
    }
    
    // Validate level index
    if (state.levelIndex < 0 || state.levelIndex >= (int)gameLevels.size() - 1) {
        return false;
    }
    
    // Reload levels fresh
    gameLevels.clear();
    ScreenParser::resetRiddleIndex();
    initGameLevels();
    ::resetUsedRiddles();
    
    // Restore progress
    currentLevelIndex = state.levelIndex;
    LivesManager::resetLives();
    LivesManager::resetCoins();
    
    // Set lives (subtract from initial to match saved)
    while (LivesManager::getLives() > state.lives && LivesManager::hasLivesRemaining()) {
        LivesManager::subtractLife();
    }
    while (LivesManager::getLives() < state.lives) {
        LivesManager::addLife();
    }
    
    // Set coins
    for (int i = 0; i < state.coins; i++) {
        LivesManager::addCoin();
    }
    
    // Restore player positions
    player1.setPosition(state.player1X, state.player1Y);
    player1.stop();
    if (state.player1Item != '\0' && state.player1Item != ' ') {
        player1.pickupItem(state.player1Item);
    } else {
        player1.disposeItem();
    }
    
    player2.setPosition(state.player2X, state.player2Y);
    player2.stop();
    if (state.player2Item != '\0' && state.player2Item != ' ') {
        player2.pickupItem(state.player2Item);
    } else {
        player2.disposeItem();
    }
    
    // Reset the current level
    if (currentLevelIndex >= 0 && currentLevelIndex < (int)gameLevels.size()) {
        gameLevels[currentLevelIndex]->reset();
    }
    
    recorder.resetCycle();
    
    return true;
}
