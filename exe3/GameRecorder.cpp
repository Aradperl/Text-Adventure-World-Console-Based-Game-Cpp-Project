// GameRecorder.cpp - Game recording and playback system
// Records inputs to steps file, events to result file, and replays for testing

#include "GameRecorder.h"
#include <iostream>
#include <sstream>
#include <ctime>
#include <algorithm>

const std::string GameRecorder::STEPS_FILE = "adv-world.steps";
const std::string GameRecorder::RESULT_FILE = "adv-world.result";

GameRecorder::GameRecorder()
    : mode(GameMode::Normal), silentMode(false),
      randomSeed(0), currentStepIndex(0), riddleAnswerIndex(0),
      currentCycle(0), riddleIndex(0) {
}

// Set recording/playback mode
void GameRecorder::setMode(GameMode m) {
    mode = m;
    silentMode = (m == GameMode::LoadSilent);
}

GameMode GameRecorder::getMode() const { return mode; }
bool GameRecorder::isSaveMode() const { return mode == GameMode::Save; }
bool GameRecorder::isLoadMode() const { return mode == GameMode::Load || mode == GameMode::LoadSilent; }
bool GameRecorder::isSilentMode() const { return mode == GameMode::LoadSilent; }
bool GameRecorder::isNormalMode() const { return mode == GameMode::Normal; }

// Track screen files used in recording
void GameRecorder::setScreenFiles(const std::vector<std::string>& files) {
    screenFiles = files;
}

const std::vector<std::string>& GameRecorder::getScreenFiles() const {
    return screenFiles;
}

// Set/get random seed for deterministic playback
void GameRecorder::setRandomSeed(unsigned int seed) {
    randomSeed = seed;
    srand(seed);
}

unsigned int GameRecorder::getRandomSeed() const {
    return randomSeed;
}

void GameRecorder::initializeRandomSeed() {
    randomSeed = static_cast<unsigned int>(time(nullptr));
    srand(randomSeed);
}

// Riddle question tracking for deterministic replay
void GameRecorder::addRiddleQuestion(const std::string& question) {
    riddleQuestions.push_back(question);
}

std::string GameRecorder::getNextRiddleQuestion() {
    if (riddleIndex < riddleQuestions.size()) {
        return riddleQuestions[riddleIndex++];
    }
    return "";  // No more riddles
}

bool GameRecorder::hasMoreRiddles() const {
    return riddleIndex < riddleQuestions.size();
}

void GameRecorder::resetRiddleIndex() {
    riddleIndex = 0;
}

// Game cycle counter
void GameRecorder::nextCycle() { currentCycle++; }
int GameRecorder::getCurrentCycle() const { return currentCycle; }
void GameRecorder::resetCycle() { currentCycle = 0; }

// Convert direction to file character
std::string GameRecorder::directionToChar(Direction dir) const {
    switch (dir) {
        case Direction::UP:    return "U";
        case Direction::DOWN:  return "D";
        case Direction::LEFT:  return "L";
        case Direction::RIGHT: return "R";
        case Direction::STAY:  return "S";
        default: return "S";
    }
}

// Parse direction from file character
Direction GameRecorder::charToDirection(char c) const {
    switch (toupper(c)) {
        case 'U': return Direction::UP;
        case 'D': return Direction::DOWN;
        case 'L': return Direction::LEFT;
        case 'R': return Direction::RIGHT;
        case 'S': return Direction::STAY;
        default:  return Direction::STAY;
    }
}

// Remove leading/trailing whitespace
std::string GameRecorder::trim(const std::string& str) const {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

// Record player movement input
void GameRecorder::recordStep(int player, Direction dir) {
    if (!isSaveMode()) return;
    
    GameStep step;
    step.cycle = currentCycle;
    step.player = player;
    step.action = directionToChar(dir)[0];
    steps.push_back(step);
}

void GameRecorder::recordStop(int player) {
    if (!isSaveMode()) return;
    
    GameStep step;
    step.cycle = currentCycle;
    step.player = player;
    step.action = 'S';
    steps.push_back(step);
}

void GameRecorder::recordDispose(int player) {
    if (!isSaveMode()) return;
    
    GameStep step;
    step.cycle = currentCycle;
    step.player = player;
    step.action = (player == 1) ? 'E' : 'O';  // Player 1 uses E, Player 2 uses O
    steps.push_back(step);
}

void GameRecorder::recordRiddleAnswer(int player, char answer) {
    if (!isSaveMode()) return;
    
    GameStep step;
    step.cycle = currentCycle;
    step.player = player;
    // Store answer as '1', '2', '3', '4' to avoid conflict with 'D' (DOWN direction)
    // 'a'/'A' -> '1', 'b'/'B' -> '2', 'c'/'C' -> '3', 'd'/'D' -> '4'
    char lowerAnswer = static_cast<char>(std::tolower(answer));
    step.action = static_cast<char>('1' + (lowerAnswer - 'a'));
    steps.push_back(step);
}

void GameRecorder::recordScreenChange(int player, int newScreen) {
    GameEvent event;
    event.cycle = currentCycle;
    event.type = "screen";
    event.player = player;
    event.data = std::to_string(newScreen);
    actualEvents.push_back(event);
}

void GameRecorder::recordLifeLost(int player) {
    GameEvent event;
    event.cycle = currentCycle;
    event.type = "life";
    event.player = player;
    event.data = "";
    actualEvents.push_back(event);
}

void GameRecorder::recordRiddle(int player, const std::string& question,
                                char answer, bool correct) {
    GameEvent event;
    event.cycle = currentCycle;
    event.type = "riddle";
    event.player = player;
    
    // Format: question|answer|correct
    std::ostringstream oss;
    oss << question << "|" << answer << "|" << (correct ? "correct" : "wrong");
    event.data = oss.str();
    actualEvents.push_back(event);
}

void GameRecorder::recordGameEnd(int score, bool victory) {
    GameEvent event;
    event.cycle = currentCycle;
    event.type = "end";
    event.player = 0;
    
    std::ostringstream oss;
    oss << (victory ? "victory" : "gameover") << "|" << score;
    event.data = oss.str();
    actualEvents.push_back(event);
}

// Check if action code is a riddle answer (1-4)
static bool isRiddleAnswerAction(char action) {
    return action == '1' || action == '2' || action == '3' || action == '4';
}

// Parse steps file for playback
bool GameRecorder::loadStepsFile() {
    std::ifstream file(STEPS_FILE);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open " << STEPS_FILE << std::endl;
        return false;
    }
    
    steps.clear();
    riddleAnswerSteps.clear();
    screenFiles.clear();
    riddleQuestions.clear();
    currentStepIndex = 0;
    riddleAnswerIndex = 0;
    riddleIndex = 0;
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;
        
        // Parse header info
        if (line.find("screens:") == 0) {
            std::string files = trim(line.substr(8));
            std::istringstream iss(files);
            std::string filename;
            while (std::getline(iss, filename, ',')) {
                screenFiles.push_back(trim(filename));
            }
            continue;
        }
        
        if (line.find("seed:") == 0) {
            randomSeed = std::stoul(trim(line.substr(5)));
            srand(randomSeed);
            continue;
        }
        
        // Parse riddle count (just skip, we read individual questions)
        if (line.find("riddles:") == 0) {
            continue;
        }
        
        // Parse riddle question
        if (line.find("riddle_q:") == 0) {
            std::string question = trim(line.substr(9));
            riddleQuestions.push_back(question);
            continue;
        }
        
        // Parse step: cycle,player,action
        std::istringstream iss(line);
        std::string token;
        
        GameStep step;
        if (std::getline(iss, token, ',')) step.cycle = std::stoi(trim(token));
        if (std::getline(iss, token, ',')) step.player = std::stoi(trim(token));
        if (std::getline(iss, token, ',')) step.action = trim(token)[0];
        
        // Separate riddle answers from movement steps
        if (isRiddleAnswerAction(step.action)) {
            riddleAnswerSteps.push_back(step);
        } else {
            steps.push_back(step);
        }
    }
    
    file.close();
    return true;
}

// Parse result file for verification
bool GameRecorder::loadResultFile() {
    std::ifstream file(RESULT_FILE);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open " << RESULT_FILE << std::endl;
        return false;
    }
    
    expectedEvents.clear();
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;
        
        // Parse event: cycle,type,player,data
        std::istringstream iss(line);
        std::string token;
        
        GameEvent event;
        if (std::getline(iss, token, ',')) event.cycle = std::stoi(trim(token));
        if (std::getline(iss, token, ',')) event.type = trim(token);
        if (std::getline(iss, token, ',')) event.player = std::stoi(trim(token));
        if (std::getline(iss, token)) event.data = trim(token);
        
        expectedEvents.push_back(event);
    }
    
    file.close();
    return true;
}

// Playback - check for more steps
bool GameRecorder::hasNextStep() const {
    return currentStepIndex < steps.size();
}

GameStep GameRecorder::getNextStep() {
    if (currentStepIndex < steps.size()) {
        return steps[currentStepIndex++];
    }
    return GameStep{-1, 0, 'S'};
}

GameStep GameRecorder::peekNextStep() const {
    if (currentStepIndex < steps.size()) {
        return steps[currentStepIndex];
    }
    return GameStep{-1, 0, 'S'};
}

bool GameRecorder::isStepReady() const {
    if (currentStepIndex >= steps.size()) return false;
    return steps[currentStepIndex].cycle == currentCycle;
}

int GameRecorder::getLastStepCycle() const {
    if (steps.empty()) return 0;
    return steps.back().cycle;
}

// Get recorded input for player at current cycle
char GameRecorder::getInputForPlayer(int player) {
    // Check all steps for this cycle and player
    while (currentStepIndex < steps.size() && 
           steps[currentStepIndex].cycle == currentCycle) {
        if (steps[currentStepIndex].player == player) {
            return steps[currentStepIndex++].action;
        }
        // Skip steps for other player
        if (steps[currentStepIndex].cycle == currentCycle) {
            currentStepIndex++;
        }
    }
    return '\0';  // No input for this player this cycle
}

// Get recorded riddle answer for player
char GameRecorder::getRiddleAnswer(int player) {
    if (!isLoadMode()) return '\0';
    
    // Get the next riddle answer from the separate queue
    if (riddleAnswerIndex < riddleAnswerSteps.size()) {
        const GameStep& step = riddleAnswerSteps[riddleAnswerIndex];
        if (step.player == player) {
            riddleAnswerIndex++;
            // Convert '1'-'4' back to 'a'-'d'
            char answer = static_cast<char>('a' + (step.action - '1'));
            return answer;
        }
    }
    return '\0';  // No riddle answer found
}

// Write recorded steps to file
bool GameRecorder::saveStepsFile() {
    std::ofstream file(STEPS_FILE);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot create " << STEPS_FILE << std::endl;
        return false;
    }
    
    // Write header
    file << "# adv-world.steps - Game recording file\n";
    file << "# Format: cycle,player,action\n";
    file << "# Actions: U=up, D=down, L=left, R=right, S=stop, E/O=dispose, 1/2/3/4=riddle answer (a/b/c/d)\n";
    file << "\n";
    
    // Write screen files used
    file << "screens: ";
    for (size_t i = 0; i < screenFiles.size(); i++) {
        if (i > 0) file << ", ";
        file << screenFiles[i];
    }
    file << "\n";
    
    // Write random seed
    file << "seed: " << randomSeed << "\n";
    
    // Write riddle questions in order they appear
    file << "riddles: " << riddleQuestions.size() << "\n";
    for (const auto& question : riddleQuestions) {
        file << "riddle_q: " << question << "\n";
    }
    file << "\n";
    
    // Write steps
    file << "# Steps (cycle, player, action)\n";
    for (const auto& step : steps) {
        file << step.cycle << "," << step.player << "," << step.action << "\n";
    }
    
    file.close();
    return true;
}

// Write game events to result file
bool GameRecorder::saveResultFile() {
    std::ofstream file(RESULT_FILE);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot create " << RESULT_FILE << std::endl;
        return false;
    }
    
    // Write header
    file << "# adv-world.result - Game results file\n";
    file << "# Format: cycle,type,player,data\n";
    file << "# Types: screen, life, riddle, end\n";
    file << "\n";
    
    // Write events
    for (const auto& event : actualEvents) {
        file << event.cycle << "," << event.type << "," 
             << event.player << "," << event.data << "\n";
    }
    
    file.close();
    return true;
}

// Compare actual events against expected from result file
bool GameRecorder::verifyResults() const {
    if (actualEvents.size() != expectedEvents.size()) {
        return false;
    }
    
    for (size_t i = 0; i < actualEvents.size(); i++) {
        const auto& actual = actualEvents[i];
        const auto& expected = expectedEvents[i];
        
        if (actual.cycle != expected.cycle ||
            actual.type != expected.type ||
            actual.player != expected.player ||
            actual.data != expected.data) {
            return false;
        }
    }
    
    return true;
}

// Print test pass/fail report with mismatches
void GameRecorder::printVerificationReport() const {
    std::cout << "\n========== VERIFICATION REPORT ==========\n";
    
    if (verifyResults()) {
        std::cout << "TEST PASSED - All events match expected results!\n";
        std::cout << "Total events verified: " << actualEvents.size() << "\n";
    } else {
        std::cout << "TEST FAILED\n\n";
        
        size_t maxEvents = std::max(actualEvents.size(), expectedEvents.size());
        
        for (size_t i = 0; i < maxEvents; i++) {
            bool hasActual = i < actualEvents.size();
            bool hasExpected = i < expectedEvents.size();
            
            bool match = hasActual && hasExpected &&
                        actualEvents[i].cycle == expectedEvents[i].cycle &&
                        actualEvents[i].type == expectedEvents[i].type &&
                        actualEvents[i].player == expectedEvents[i].player &&
                        actualEvents[i].data == expectedEvents[i].data;
            
            if (!match) {
                std::cout << "Mismatch at event " << i << ":\n";
                
                if (hasExpected) {
                    std::cout << "  Expected: cycle=" << expectedEvents[i].cycle
                              << ", type=" << expectedEvents[i].type
                              << ", player=" << expectedEvents[i].player
                              << ", data=" << expectedEvents[i].data << "\n";
                } else {
                    std::cout << "  Expected: <none>\n";
                }
                
                if (hasActual) {
                    std::cout << "  Actual:   cycle=" << actualEvents[i].cycle
                              << ", type=" << actualEvents[i].type
                              << ", player=" << actualEvents[i].player
                              << ", data=" << actualEvents[i].data << "\n";
                } else {
                    std::cout << "  Actual:   <none>\n";
                }
                
                std::cout << "\n";
            }
        }
        
        std::cout << "Expected " << expectedEvents.size() << " events, got " 
                  << actualEvents.size() << " events.\n";
    }
    
    std::cout << "==========================================\n";
}

// Reset all recorded data
void GameRecorder::clear() {
    steps.clear();
    riddleAnswerSteps.clear();
    actualEvents.clear();
    expectedEvents.clear();
    riddleQuestions.clear();
    currentStepIndex = 0;
    riddleAnswerIndex = 0;
    riddleIndex = 0;
    currentCycle = 0;
}

// Debug output - print all recorded steps
void GameRecorder::printSteps() const {
    std::cout << "=== Recorded Steps ===\n";
    for (const auto& step : steps) {
        std::cout << "Cycle " << step.cycle << ": Player " << step.player 
                  << " -> " << step.action << "\n";
    }
}

// Debug output - print all recorded events
void GameRecorder::printEvents() const {
    std::cout << "=== Recorded Events ===\n";
    for (const auto& event : actualEvents) {
        std::cout << "Cycle " << event.cycle << ": " << event.type 
                  << " (Player " << event.player << ") - " << event.data << "\n";
    }
}

