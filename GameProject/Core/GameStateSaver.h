// GameStateSaver.h - Save and load game state

#pragma once

#include <string>
#include <vector>

// Information about a saved game (for display in load menu)
struct SaveInfo {
    std::string filename;
    std::string timestamp;  // Human-readable date/time
    int level;              // Level number when saved
};

// Complete game state for saving/loading
struct SavedGameState {
    // Timestamp
    std::string timestamp;
    
    // Progress
    int levelIndex;         // 0-based level index
    int lives;
    int coins;
    
    // Player 1 state
    int player1X;
    int player1Y;
    char player1Item;       // Held item ('\0' if none)
    
    // Player 2 state
    int player2X;
    int player2Y;
    char player2Item;       // Held item ('\0' if none)
    
    // Per-level state tracking (positions of collected/used items)
    std::vector<std::pair<int, int>> collectedKeys;
    std::vector<std::pair<int, int>> collectedCoins;
    std::vector<char> openedGates;
    std::vector<std::pair<int, int>> solvedRiddles;
    
    SavedGameState();
};

class GameStateSaver {
private:
    static const std::string SAVE_EXTENSION;
    static const std::string SAVE_PREFIX;
    
    // Helper methods
    static std::string generateFilename();
    static std::string getCurrentTimestamp();
    static bool parseKeyValue(const std::string& line, std::string& key, std::string& value);
    static std::vector<std::pair<int, int>> parsePointList(const std::string& str);
    static std::string pointListToString(const std::vector<std::pair<int, int>>& points);
    static std::vector<char> parseCharList(const std::string& str);
    static std::string charListToString(const std::vector<char>& chars);

public:
    // Save current game state to a new file
    static bool saveGameState(const SavedGameState& state);
    
    // Load game state from a specific file
    static bool loadGameState(const std::string& filename, SavedGameState& state);
    
    // Get list of all available save files (sorted by date, newest first)
    static std::vector<SaveInfo> getSaveFiles();
    
    // Delete a save file
    static bool deleteSave(const std::string& filename);
};

