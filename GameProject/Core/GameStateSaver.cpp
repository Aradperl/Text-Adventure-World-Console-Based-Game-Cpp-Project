// GameStateSaver.cpp - Save and load game progress to files

#include "GameStateSaver.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <filesystem>
#include <iomanip>

namespace fs = std::filesystem;

const std::string GameStateSaver::SAVE_EXTENSION = ".sav";
const std::string GameStateSaver::SAVE_PREFIX = "adv-world_";

// SavedGameState constructor
SavedGameState::SavedGameState()
    : levelIndex(0), lives(4), coins(0),
      player1X(2), player1Y(2), player1Item('\0'),
      player2X(2), player2Y(18), player2Item('\0') {}

// Generate filename with current timestamp
std::string GameStateSaver::generateFilename() {
    time_t now = time(nullptr);
    tm ltm;
    localtime_s(&ltm, &now);
    
    std::ostringstream oss;
    oss << SAVE_PREFIX
        << (1900 + ltm.tm_year) << "-"
        << std::setfill('0') << std::setw(2) << (1 + ltm.tm_mon) << "-"
        << std::setfill('0') << std::setw(2) << ltm.tm_mday << "_"
        << std::setfill('0') << std::setw(2) << ltm.tm_hour << "-"
        << std::setfill('0') << std::setw(2) << ltm.tm_min << "-"
        << std::setfill('0') << std::setw(2) << ltm.tm_sec
        << SAVE_EXTENSION;
    
    return oss.str();
}

// Get current timestamp as human-readable string
std::string GameStateSaver::getCurrentTimestamp() {
    time_t now = time(nullptr);
    tm ltm;
    localtime_s(&ltm, &now);
    
    std::ostringstream oss;
    oss << (1900 + ltm.tm_year) << "-"
        << std::setfill('0') << std::setw(2) << (1 + ltm.tm_mon) << "-"
        << std::setfill('0') << std::setw(2) << ltm.tm_mday << " "
        << std::setfill('0') << std::setw(2) << ltm.tm_hour << ":"
        << std::setfill('0') << std::setw(2) << ltm.tm_min << ":"
        << std::setfill('0') << std::setw(2) << ltm.tm_sec;
    
    return oss.str();
}

// Parse a key=value line
bool GameStateSaver::parseKeyValue(const std::string& line, std::string& key, std::string& value) {
    size_t pos = line.find('=');
    if (pos == std::string::npos) return false;
    
    key = line.substr(0, pos);
    value = line.substr(pos + 1);
    
    // Trim whitespace
    while (!key.empty() && (key.back() == ' ' || key.back() == '\t')) key.pop_back();
    while (!value.empty() && (value.front() == ' ' || value.front() == '\t')) value.erase(0, 1);
    
    return true;
}

// Parse point list like "5,8;12,4" into vector of pairs
std::vector<std::pair<int, int>> GameStateSaver::parsePointList(const std::string& str) {
    std::vector<std::pair<int, int>> points;
    if (str.empty()) return points;
    
    std::istringstream iss(str);
    std::string point;
    
    while (std::getline(iss, point, ';')) {
        size_t comma = point.find(',');
        if (comma != std::string::npos) {
            int x = std::stoi(point.substr(0, comma));
            int y = std::stoi(point.substr(comma + 1));
            points.push_back({x, y});
        }
    }
    
    return points;
}

// Convert point list to string
std::string GameStateSaver::pointListToString(const std::vector<std::pair<int, int>>& points) {
    std::ostringstream oss;
    for (size_t i = 0; i < points.size(); i++) {
        if (i > 0) oss << ";";
        oss << points[i].first << "," << points[i].second;
    }
    return oss.str();
}

// Parse char list like "A;B;C"
std::vector<char> GameStateSaver::parseCharList(const std::string& str) {
    std::vector<char> chars;
    if (str.empty()) return chars;
    
    std::istringstream iss(str);
    std::string item;
    
    while (std::getline(iss, item, ';')) {
        if (!item.empty()) {
            chars.push_back(item[0]);
        }
    }
    
    return chars;
}

// Convert char list to string
std::string GameStateSaver::charListToString(const std::vector<char>& chars) {
    std::ostringstream oss;
    for (size_t i = 0; i < chars.size(); i++) {
        if (i > 0) oss << ";";
        oss << chars[i];
    }
    return oss.str();
}

// Save game state to file
bool GameStateSaver::saveGameState(const SavedGameState& state) {
    std::string filename = generateFilename();
    std::ofstream file(filename);
    
    if (!file.is_open()) return false;
    
    // Write header
    file << "timestamp=" << getCurrentTimestamp() << "\n";
    file << "level=" << (state.levelIndex + 1) << "\n";  // Display as 1-based
    file << "lives=" << state.lives << "\n";
    file << "coins=" << state.coins << "\n";
    
    // Player 1 state
    file << "player1_x=" << state.player1X << "\n";
    file << "player1_y=" << state.player1Y << "\n";
    file << "player1_item=" << (state.player1Item ? state.player1Item : ' ') << "\n";
    
    // Player 2 state
    file << "player2_x=" << state.player2X << "\n";
    file << "player2_y=" << state.player2Y << "\n";
    file << "player2_item=" << (state.player2Item ? state.player2Item : ' ') << "\n";
    
    // Level state section
    file << "[level_state]\n";
    file << "collected_keys=" << pointListToString(state.collectedKeys) << "\n";
    file << "collected_coins=" << pointListToString(state.collectedCoins) << "\n";
    file << "opened_gates=" << charListToString(state.openedGates) << "\n";
    file << "solved_riddles=" << pointListToString(state.solvedRiddles) << "\n";
    
    file.close();
    return true;
}

// Load game state from file
bool GameStateSaver::loadGameState(const std::string& filename, SavedGameState& state) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    
    std::string line;
    bool inLevelState = false;
    
    while (std::getline(file, line)) {
        // Skip empty lines
        if (line.empty()) continue;
        
        // Check for section header
        if (line == "[level_state]") {
            inLevelState = true;
            continue;
        }
        
        std::string key, value;
        if (!parseKeyValue(line, key, value)) continue;
        
        if (!inLevelState) {
            // Main section
            if (key == "timestamp") state.timestamp = value;
            else if (key == "level") state.levelIndex = std::stoi(value) - 1;  // Convert to 0-based
            else if (key == "lives") state.lives = std::stoi(value);
            else if (key == "coins") state.coins = std::stoi(value);
            else if (key == "player1_x") state.player1X = std::stoi(value);
            else if (key == "player1_y") state.player1Y = std::stoi(value);
            else if (key == "player1_item") state.player1Item = (value.empty() || value[0] == ' ') ? '\0' : value[0];
            else if (key == "player2_x") state.player2X = std::stoi(value);
            else if (key == "player2_y") state.player2Y = std::stoi(value);
            else if (key == "player2_item") state.player2Item = (value.empty() || value[0] == ' ') ? '\0' : value[0];
        } else {
            // Level state section
            if (key == "collected_keys") state.collectedKeys = parsePointList(value);
            else if (key == "collected_coins") state.collectedCoins = parsePointList(value);
            else if (key == "opened_gates") state.openedGates = parseCharList(value);
            else if (key == "solved_riddles") state.solvedRiddles = parsePointList(value);
        }
    }
    
    file.close();
    return true;
}

// Get list of all save files
std::vector<SaveInfo> GameStateSaver::getSaveFiles() {
    std::vector<SaveInfo> saves;
    
    try {
        for (const auto& entry : fs::directory_iterator(".")) {
            if (!entry.is_regular_file()) continue;
            
            std::string filename = entry.path().filename().string();
            
            // Check if it's a save file
            if (filename.size() > SAVE_EXTENSION.size() &&
                filename.substr(filename.size() - SAVE_EXTENSION.size()) == SAVE_EXTENSION &&
                filename.substr(0, SAVE_PREFIX.size()) == SAVE_PREFIX) {
                
                SaveInfo info;
                info.filename = filename;
                
                // Load just the header to get timestamp and level
                SavedGameState state;
                if (loadGameState(filename, state)) {
                    info.timestamp = state.timestamp;
                    info.level = state.levelIndex + 1;  // 1-based for display
                }
                
                saves.push_back(info);
            }
        }
    } catch (...) {
        // Directory iteration failed, return empty list
    }
    
    // Sort by filename (which includes timestamp) - newest first
    std::sort(saves.begin(), saves.end(), [](const SaveInfo& a, const SaveInfo& b) {
        return a.filename > b.filename;
    });
    
    return saves;
}

// Delete a save file
bool GameStateSaver::deleteSave(const std::string& filename) {
    try {
        return fs::remove(filename);
    } catch (...) {
        return false;
    }
}

