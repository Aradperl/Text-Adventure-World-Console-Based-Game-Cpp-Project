// GameRecorder.h - Game recording and playback system for testing
// Records game inputs to adv-world.steps and events to adv-world.result

#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <map>
#include "Direction.h"

// Game execution modes
enum class GameMode {
    Normal,     // Standard gameplay (Ex2 behavior)
    Save,       // Record gameplay to files
    Load,       // Playback from files
    LoadSilent  // Playback without display, just verify results
};

// Input step (player action at specific time)
struct GameStep {
    int cycle;          // Game cycle when input occurred
    int player;         // 1 or 2
    char action;        // 'U','D','L','R','S' (directions/stop), 'E'/'O' (dispose), or '1'-'4' (riddle answer a-d)
};

// Game event for results file
struct GameEvent {
    int cycle;
    std::string type;   // "screen", "life", "riddle", "end"
    int player;         // Which player (0 if N/A)
    std::string data;   // Additional event-specific data
};

class GameRecorder {
private:
    // Mode
    GameMode mode;
    bool silentMode;
    
    // File paths
    static const std::string STEPS_FILE;
    static const std::string RESULT_FILE;
    
    // Screen files used in this session
    std::vector<std::string> screenFiles;
    
    // Random seed for reproducibility
    unsigned int randomSeed;
    
    // Recorded/loaded steps (movement only)
    std::vector<GameStep> steps;
    size_t currentStepIndex;
    
    // Riddle answers stored separately for clean handling
    std::vector<GameStep> riddleAnswerSteps;
    size_t riddleAnswerIndex;
    
    // Recorded events (actual)
    std::vector<GameEvent> actualEvents;
    
    // Expected events (loaded from file)
    std::vector<GameEvent> expectedEvents;
    
    // Current game cycle
    int currentCycle;
    
    // Riddle questions in order they appear (for reproducibility)
    std::vector<std::string> riddleQuestions;
    size_t riddleIndex;
    
    // Helper methods
    std::string directionToChar(Direction dir) const;
    Direction charToDirection(char c) const;
    std::string trim(const std::string& str) const;
    
public:
    GameRecorder();
    
    // Mode management
    void setMode(GameMode m);
    GameMode getMode() const;
    bool isSaveMode() const;
    bool isLoadMode() const;
    bool isSilentMode() const;
    bool isNormalMode() const;
    
    // Screen files tracking
    void setScreenFiles(const std::vector<std::string>& files);
    const std::vector<std::string>& getScreenFiles() const;
    
    // Random seed management
    void setRandomSeed(unsigned int seed);
    unsigned int getRandomSeed() const;
    void initializeRandomSeed();
    
    // Riddle question management (for exact reproducibility)
    void addRiddleQuestion(const std::string& question);
    std::string getNextRiddleQuestion();
    bool hasMoreRiddles() const;
    void resetRiddleIndex();
    
    // Cycle management
    void nextCycle();
    int getCurrentCycle() const;
    void resetCycle();
    
    // Recording (save mode)
    void recordStep(int player, Direction dir);
    void recordStop(int player);
    void recordDispose(int player);
    void recordRiddleAnswer(int player, char answer);  // Record riddle answer as step
    void recordScreenChange(int player, int newScreen);
    void recordLifeLost(int player);
    void recordRiddle(int player, const std::string& question, 
                      char answer, bool correct);
    void recordGameEnd(int score, bool victory);
    
    // Playback (load mode)
    bool loadStepsFile();
    bool loadResultFile();
    bool hasNextStep() const;
    GameStep getNextStep();
    GameStep peekNextStep() const;
    bool isStepReady() const;  // True if next step's cycle matches current
    int getLastStepCycle() const;  // Get cycle of last recorded step
    
    // Get input for current cycle (returns '\0' if none)
    char getInputForPlayer(int player);
    
    // Get riddle answer for playback (returns the recorded answer, or '\0' if none)
    char getRiddleAnswer(int player);
    
    // File operations
    bool saveStepsFile();
    bool saveResultFile();
    
    // Verification (silent mode)
    bool verifyResults() const;
    void printVerificationReport() const;
    
    // Clear recorded data
    void clear();
    
    // Debug
    void printSteps() const;
    void printEvents() const;
};

