// ScreenParser.h - Visual ASCII-based screen loading
// Screens are defined by literally drawing them in text files!

#pragma once

#include "GameLevel.h"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <sstream>

// ============================================================
// SCREEN FILE FORMAT
// ============================================================
//
// FILES:
//   Screen files: adv-world_XX.screen (loaded lexicographically)
//   Riddles file: riddles.txt (contains all riddle definitions)
//
// CHARACTERS:
//   W       = Wall
//   (space) = Empty
//   @       = Player 1 start position
//   &       = Player 2 start position
//   K       = Key
//   C       = Coin (collectible, adds to coin counter)
//   ?       = Riddle (ID specified in metadata, content from riddles.txt)
//   \       = Switch (off)
//   /       = Switch (on)
//   !       = Torch
//   S       = Light switch
//   0-9     = Door number (connects to screen with that number)
//   |, -    = Gate bars (auto-detected as connected region)
//   D       = Dark room marker
//   B       = Bomb (collectible, explodes after 5 cycles when dropped)
//   *       = Obstacle (pushable, adjacent * form one obstacle)
//   #       = Spring (launches player, must be adjacent to wall)
//
// METADATA (comments at top of file):
//   # gate <char>                  - which door the gate blocks
//   # switch <x>,<y> gate: <char>  - link switch to gate
//
// RIDDLES FILE (riddles.txt):
//   [riddle_id]
//   Q: Question text
//   A: Option 1
//   A: Option 2
//   A: Option 3
//   A: Option 4
//   S: correct_answer (a/b/c/d or multiple: a,c)
//
// ============================================================

// Riddle definition loaded from riddles.txt
struct RiddleDefinition {
    std::string id;
    std::string question;
    std::vector<std::string> options;
    std::vector<char> correctAnswers;  // Supports multiple correct answers
};

class ScreenParser {
private:
    // Parsed metadata
    int levelNumber;
    bool darkRoom;
    int torchRadius;
    std::string errorMessage;

    // Legend position
    int legendX, legendY;
    bool hasLegend;

    // Player start positions
    int player1X, player1Y;
    int player2X, player2Y;
    bool hasPlayer1Start, hasPlayer2Start;

    // Door targets: door_char -> target_level
    std::map<char, int> doorTargets;

    // Switch linkages: "x,y" -> gate_char
    std::map<std::string, char> switchGates;

    // Gate door numbers (which door char the gate blocks)
    char gateChar;

    // Riddle positions: "x,y" -> riddle_id
    std::map<std::string, std::string> riddleIds;

    // Loaded riddles from riddles.txt
    static std::map<std::string, RiddleDefinition> loadedRiddles;  // For lookup by ID
    static std::vector<std::string> riddleIdOrder;  // Preserves file order (stores IDs)
    static bool riddlesLoaded;
    
    // Sequential riddle index (riddles assigned in order from riddles.txt)
    static size_t currentRiddleIndex;
    
    // Recorder for load mode (to get pre-recorded riddle questions)
    static class GameRecorder* recorder;

    // Helper methods
    std::string trim(const std::string& str) const;
    std::string toLower(const std::string& str) const;
    bool parseMetadata(const std::string& line);
    void parseVisualLine(const std::string& line, int y, GameLevel* level);
    void detectAndAddGates(const std::vector<std::string>& lines, GameLevel* level);
    bool isGateChar(char c) const;

    // Riddle loading
    static void loadRiddlesFile();
    RiddleDefinition* findRiddle(const std::string& id);
    static RiddleDefinition* getNextRiddle();  // Sequential riddle selection

public:
    ScreenParser();

    // Load a screen from file
    std::unique_ptr<GameLevel> loadFromFile(const std::string& filename);
    std::unique_ptr<GameLevel> loadFromFile(const std::string& filename, const std::set<std::string>& usedRiddles);

    // Get last error message
    std::string getError() const;

    // Discover all screen files (adv-world*.screen) in lexicographical order
    static std::vector<std::string> discoverScreenFiles();

    // Extract screen number from filename (e.g., "adv-world_02.screen" -> 2)
    static int getScreenNumber(const std::string& filename);

    // Get legend position (call after loadFromFile)
    bool getLegendPosition(int& x, int& y) const;

    // Get player start positions (call after loadFromFile)
    bool getPlayer1Start(int& x, int& y) const;
    bool getPlayer2Start(int& x, int& y) const;

    // Reload riddles from file
    static void reloadRiddles();
    
    // Reset riddle index for new game (riddles are assigned in order)
    static void resetRiddleIndex();
    
    // Set recorder for load mode (to get pre-recorded riddle questions)
    static void setRecorder(GameRecorder* rec);
    
    // Find riddle by question text
    static RiddleDefinition* findRiddleByQuestion(const std::string& question);
};
