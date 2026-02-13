// ScreenParser.cpp - Loads level data from ASCII screen files
// Parses .screen files into GameLevel objects with objects, metadata, and riddles
// AI usage: Regex level number extraction (AI-assisted)

#include "ScreenParser.h"
#include "Game.h"
#include "GameConfig.h"
#include "GameRecorder.h"
#include <algorithm>
#include <iostream>
#include <set>
#include <filesystem>
#include <regex>
#include <random>
#include <ctime>
#include <direct.h>
#include <fstream>

std::map<std::string, RiddleDefinition> ScreenParser::loadedRiddles;
std::vector<std::string> ScreenParser::riddleIdOrder;
bool ScreenParser::riddlesLoaded = false;
size_t ScreenParser::currentRiddleIndex = 0;
GameRecorder* ScreenParser::recorder = nullptr;

void ScreenParser::setRecorder(GameRecorder* rec) {
    recorder = rec;
}

ScreenParser::ScreenParser()
    : levelNumber(1), darkRoom(false), torchRadius(3),
    legendX(-1), legendY(-1), hasLegend(false),
    player1X(2), player1Y(2), player2X(2), player2Y(18),
    hasPlayer1Start(false), hasPlayer2Start(false),
    gateChar('0') {
}

std::string ScreenParser::trim(const std::string& str) const {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

std::string ScreenParser::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Parse riddles.txt and cache riddle definitions
void ScreenParser::loadRiddlesFile() {
    // #region agent log H1-H3: Check if using cached riddles
    { std::ofstream dbg("c:\\Users\\admin\\source\\repos\\GameProject\\Exe_2\\.cursor\\debug.log", std::ios::app); dbg << "{\"location\":\"ScreenParser.cpp:loadRiddlesFile\",\"message\":\"loadRiddlesFile called\",\"data\":{\"riddlesLoaded\":" << (riddlesLoaded ? "true" : "false") << "},\"hypothesisId\":\"H3\",\"timestamp\":" << time(nullptr) << "}\n"; }
    // #endregion
    if (riddlesLoaded) return;

    // #region agent log H2: Log working directory and file path
    { std::ofstream dbg("c:\\Users\\admin\\source\\repos\\GameProject\\Exe_2\\.cursor\\debug.log", std::ios::app); char cwd[256]; _getcwd(cwd, 256); dbg << "{\"location\":\"ScreenParser.cpp:loadRiddlesFile\",\"message\":\"Opening riddles.txt\",\"data\":{\"cwd\":\"" << cwd << "\"},\"hypothesisId\":\"H2\",\"timestamp\":" << time(nullptr) << "}\n"; }
    // #endregion
    std::ifstream file("riddles.txt");
    if (!file.is_open()) {
        // #region agent log H5: File open failed
        { std::ofstream dbg("c:\\Users\\admin\\source\\repos\\GameProject\\Exe_2\\.cursor\\debug.log", std::ios::app); dbg << "{\"location\":\"ScreenParser.cpp:loadRiddlesFile\",\"message\":\"riddles.txt NOT FOUND\",\"hypothesisId\":\"H5\",\"timestamp\":" << time(nullptr) << "}\n"; }
        // #endregion
        std::cerr << "Warning: riddles.txt not found" << std::endl;
        riddlesLoaded = true;
        return;
    }

    std::string line;
    RiddleDefinition* current = nullptr;

    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        std::string trimmed = line;
        size_t first = trimmed.find_first_not_of(" \t");
        if (first != std::string::npos) {
            trimmed = trimmed.substr(first);
        }

        if (trimmed.empty() || trimmed[0] == '#') continue;

        // Section header: [riddle_id]
        if (trimmed[0] == '[' && trimmed.back() == ']') {
            std::string id = trimmed.substr(1, trimmed.length() - 2);
            loadedRiddles[id] = RiddleDefinition();
            loadedRiddles[id].id = id;
            current = &loadedRiddles[id];
            // Store ID in order for sequential access
            riddleIdOrder.push_back(id);
            continue;
        }

        if (!current) continue;

        // Q: question text
        if (trimmed.length() > 2 && (trimmed[0] == 'Q' || trimmed[0] == 'q') && trimmed[1] == ':') {
            current->question = trimmed.substr(2);
            size_t qFirst = current->question.find_first_not_of(" \t");
            if (qFirst != std::string::npos) {
                current->question = current->question.substr(qFirst);
            }
            continue;
        }

        // A: answer option
        if (trimmed.length() > 2 && (trimmed[0] == 'A' || trimmed[0] == 'a') && trimmed[1] == ':') {
            std::string opt = trimmed.substr(2);
            size_t oFirst = opt.find_first_not_of(" \t");
            if (oFirst != std::string::npos) {
                opt = opt.substr(oFirst);
            }
            current->options.push_back(opt);
            continue;
        }

        // S: correct solution(s)
        if (trimmed.length() > 2 && (trimmed[0] == 'S' || trimmed[0] == 's') && trimmed[1] == ':') {
            std::string sol = trimmed.substr(2);
            for (char c : sol) {
                if (c >= 'a' && c <= 'd') {
                    current->correctAnswers.push_back(c);
                }
                else if (c >= 'A' && c <= 'D') {
                    current->correctAnswers.push_back(c - 'A' + 'a');
                }
            }
            continue;
        }
    }

    file.close();
    // #region agent log H3-H4: Log loaded riddles
    { std::ofstream dbg("c:\\Users\\admin\\source\\repos\\GameProject\\Exe_2\\.cursor\\debug.log", std::ios::app); dbg << "{\"location\":\"ScreenParser.cpp:loadRiddlesFile\",\"message\":\"Riddles loaded\",\"data\":{\"count\":" << loadedRiddles.size() << ",\"first_question\":\"" << (loadedRiddles.empty() ? "none" : loadedRiddles.begin()->second.question.substr(0,50)) << "\"},\"hypothesisId\":\"H3\",\"timestamp\":" << time(nullptr) << "}\n"; }
    // #endregion
    riddlesLoaded = true;
}

void ScreenParser::reloadRiddles() {
    loadedRiddles.clear();
    riddleIdOrder.clear();
    riddlesLoaded = false;
    currentRiddleIndex = 0;
    loadRiddlesFile();
}

RiddleDefinition* ScreenParser::findRiddle(const std::string& id) {
    loadRiddlesFile();
    auto it = loadedRiddles.find(id);
    if (it != loadedRiddles.end()) {
        return &it->second;
    }
    return nullptr;
}

// Look up riddle by question text (for load mode matching)
RiddleDefinition* ScreenParser::findRiddleByQuestion(const std::string& question) {
    loadRiddlesFile();
    
    for (auto& pair : loadedRiddles) {
        if (pair.second.question == question) {
            return &pair.second;
        }
    }
    return nullptr;
}

// Reset riddle counter for new game
void ScreenParser::resetRiddleIndex() {
    currentRiddleIndex = 0;
}

// Get next riddle in file order (or from recording in load mode)
RiddleDefinition* ScreenParser::getNextRiddle() {
    loadRiddlesFile();

    if (riddleIdOrder.empty()) {
        return nullptr;
    }

    // In load mode, use the pre-recorded riddle questions
    if (recorder && recorder->isLoadMode() && recorder->hasMoreRiddles()) {
        std::string question = recorder->getNextRiddleQuestion();
        RiddleDefinition* riddle = findRiddleByQuestion(question);
        if (riddle) {
            return riddle;
        }
        // Fall back to sequential if not found
    }
    
    // Sequential riddle selection: use riddles in FILE ORDER from riddles.txt
    // Wrap around if there are more '?' markers than riddles
    size_t index = currentRiddleIndex % riddleIdOrder.size();
    const std::string& riddleId = riddleIdOrder[index];
    currentRiddleIndex++;
    
    auto it = loadedRiddles.find(riddleId);
    if (it == loadedRiddles.end()) {
        return nullptr;
    }
    
    // In save mode, record the selected riddle question
    if (recorder && recorder->isSaveMode()) {
        recorder->addRiddleQuestion(it->second.question);
    }

    return &it->second;
}

// Parse metadata directives (dark_room, torch_radius, door targets, etc.)
bool ScreenParser::parseMetadata(const std::string& line) {
    if (line.empty() || line[0] != '#') return false;

    std::string content = trim(line.substr(1));
    std::string lower = toLower(content);

    if (lower.find("dark_room:") == 0) {
        std::string val = toLower(trim(content.substr(10)));
        darkRoom = (val == "true" || val == "1" || val == "yes");
        return true;
    }

    if (lower.find("torch_radius:") == 0) {
        torchRadius = std::stoi(trim(content.substr(13)));
        return true;
    }

    // door 2 target: 3
    if (lower.find("door ") == 0 && lower.find("target:") != std::string::npos) {
        char doorChar = content[5];
        size_t targetPos = lower.find("target:") + 7;
        int target = std::stoi(trim(content.substr(targetPos)));
        doorTargets[doorChar] = target;
        return true;
    }

    // switch 30,14 gate: 2
    if (lower.find("switch ") == 0 && lower.find("gate:") != std::string::npos) {
        size_t coordStart = 7;
        size_t gatePos = lower.find("gate:");
        std::string coords = trim(content.substr(coordStart, gatePos - coordStart));
        char linkedGate = trim(content.substr(gatePos + 5))[0];
        switchGates[coords] = linkedGate;
        return true;
    }

    // gate 2
    if (lower.find("gate ") == 0 && lower.find("gate:") == std::string::npos) {
        gateChar = trim(content.substr(5))[0];
        return true;
    }

    // riddle 32,16 id: math1
    if (lower.find("riddle ") == 0 && lower.find("id:") != std::string::npos) {
        size_t coordStart = 7;
        size_t idPos = lower.find("id:");
        std::string coords = trim(content.substr(coordStart, idPos - coordStart));
        std::string riddleId = trim(content.substr(idPos + 3));
        riddleIds[coords] = riddleId;
        return true;
    }

    return false;
}

bool ScreenParser::isGateChar(char c) const {
    return c == '|' || c == '-';
}

// Detect gate bounds from visual | and - characters
void ScreenParser::detectAndAddGates(const std::vector<std::string>& lines, GameLevel* level) {
    int minX = GameConfig::GAME_WIDTH, minY = GameConfig::GAME_HEIGHT;
    int maxX = 0, maxY = 0;
    bool foundGate = false;

    for (int y = 0; y < (int)lines.size() && y < GameConfig::GAME_HEIGHT; y++) {
        for (int x = 0; x < (int)lines[y].size() && x < GameConfig::GAME_WIDTH; x++) {
            if (isGateChar(lines[y][x])) {
                foundGate = true;
                if (x < minX) minX = x;
                if (y < minY) minY = y;
                if (x > maxX) maxX = x;
                if (y > maxY) maxY = y;
            }
        }
    }

    if (foundGate) {
        int width = maxX - minX + 1;
        int height = maxY - minY + 1;
        level->addGate(minX, minY, width, height, gateChar);
    }
}

// Parse single line of level data and create corresponding objects
void ScreenParser::parseVisualLine(const std::string& line, int y, GameLevel* level) {
    for (int x = 0; x < (int)line.size() && x < GameConfig::GAME_WIDTH; x++) {
        char c = line[x];

        switch (c) {
        case 'W':
            level->addWall(x, y);
            break;

        case 'K':
            level->addKey(x, y);
            break;

        case '!':
            level->addTorch(x, y, torchRadius);
            break;

        case 'S':
            level->addLightSwitch(x, y, false);
            break;

        case '$':  // Player 1 spawn
            player1X = x;
            player1Y = y;
            hasPlayer1Start = true;
            break;

        case '&':  // Player 2 spawn
            player2X = x;
            player2Y = y;
            hasPlayer2Start = true;
            break;

        case '\\':
        case '/': {
            std::string coordKey = std::to_string(x) + "," + std::to_string(y);
            char linkedGate = gateChar;
            auto it = switchGates.find(coordKey);
            if (it != switchGates.end()) {
                linkedGate = it->second;
            }
            level->addSwitch(x, y, linkedGate, c == '/');
            break;
        }

        case '?': {
            RiddleDefinition* rd = getNextRiddle();
            if (rd && !rd->question.empty() && !rd->options.empty()) {
                char answer = rd->correctAnswers.empty() ? 'a' : rd->correctAnswers[0];
                level->addRiddle(x, y, rd->question, rd->options, answer);
            }
            break;
        }

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9': {
            int target = c - '0';
            auto it = doorTargets.find(c);
            if (it != doorTargets.end()) {
                target = it->second;
            }
            level->addDoor(x, y, c, target);
            break;
        }

        case 'C':
            level->addCoin(x, y);
            break;

        case 'D':
            darkRoom = true;
            break;

        case '@':
            level->addBomb(x, y);
            break;
            
        case '*':
            level->addObstacleCell(x, y);
            break;
            
        case '#':
            level->addSpringCell(x, y);
            break;

        case '|':
        case '-':
            // Gate bars handled by detectAndAddGates
            break;

        case ' ':
        default:
            break;
        }
    }
}

// Load and parse screen file into a GameLevel
std::unique_ptr<GameLevel> ScreenParser::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        errorMessage = "Could not open file: " + filename;
        return nullptr;
    }

    // Reset parsing state
    levelNumber = getScreenNumber(filename);
    darkRoom = false;
    torchRadius = 3;
    legendX = legendY = -1;
    hasLegend = false;
    player1X = 2; player1Y = 2;
    player2X = 2; player2Y = 18;
    hasPlayer1Start = hasPlayer2Start = false;
    gateChar = '0';
    doorTargets.clear();
    switchGates.clear();
    riddleIds.clear();

    std::vector<std::string> visualLines;
    std::string line;

    // First pass: collect lines and metadata
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (!line.empty() && line[0] == '#') {
            parseMetadata(line);
        }
        else {
            visualLines.push_back(line);
        }
    }

    file.close();

    auto level = std::make_unique<GameLevel>(levelNumber);
    level->setDarkRoom(darkRoom);
    level->setTorchRadius(torchRadius);

    // Second pass: create objects from visual data
    for (int y = 0; y < (int)visualLines.size() && y < GameConfig::GAME_HEIGHT; y++) {
        parseVisualLine(visualLines[y], y, level.get());
    }

    detectAndAddGates(visualLines, level.get());

    return level;
}

// Load screen file and filter out used riddles for fresh gameplay
std::unique_ptr<GameLevel> ScreenParser::loadFromFile(const std::string& filename, const std::set<std::string>& usedRiddles) {
    auto level = loadFromFile(filename);

    if (!level) return level;

    auto& riddles = level->getRiddles();
    std::vector<std::unique_ptr<Riddle>> riddlesToKeep;

    for (auto it = riddles.begin(); it != riddles.end(); ) {
        if (*it && !::isRiddleUsed((*it)->getQuestion())) {
            riddlesToKeep.push_back(std::move(*it));
            it = riddles.erase(it);
        } else {
            ++it;
        }
    }

    for (auto& riddle : riddlesToKeep) {
        if (riddle) {
            level->addRiddle(std::move(riddle));
        }
    }

    return level;
}

std::string ScreenParser::getError() const { return errorMessage; }

bool ScreenParser::getLegendPosition(int& x, int& y) const {
    if (hasLegend) {
        x = legendX;
        y = legendY;
        return true;
    }
    return false;
}

bool ScreenParser::getPlayer1Start(int& x, int& y) const {
    x = player1X;
    y = player1Y;
    return hasPlayer1Start;
}

bool ScreenParser::getPlayer2Start(int& x, int& y) const {
    x = player2X;
    y = player2Y;
    return hasPlayer2Start;
}

// Discover and sort all adv-world_XX.screen files in directory
std::vector<std::string> ScreenParser::discoverScreenFiles() {
    std::vector<std::string> files;

    try {
        for (const auto& entry : std::filesystem::directory_iterator(".")) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();

                if (filename.find("adv-world") == 0 &&
                    filename.size() > 7 &&
                    filename.substr(filename.size() - 7) == ".screen") {
                    files.push_back(filename);
                }
            }
        }
    }
    catch (const std::filesystem::filesystem_error&) {
        // Directory iteration failed
    }

    std::sort(files.begin(), files.end());
    return files;
}

// AI-assisted: Extract level number from filename using regex
int ScreenParser::getScreenNumber(const std::string& filename) {
    std::regex pattern("adv-world[_-]?(\\d+)\\.screen");
    std::smatch match;

    if (std::regex_match(filename, match, pattern) && match.size() > 1) {
        return std::stoi(match[1].str());
    }

    // Fallback: just grab any digits
    std::string digits;
    for (char c : filename) {
        if (std::isdigit(c)) {
            digits += c;
        }
    }

    if (!digits.empty()) {
        return std::stoi(digits);
    }

    return 0;
}
