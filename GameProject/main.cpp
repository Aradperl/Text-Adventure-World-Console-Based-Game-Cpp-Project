// main.cpp - Application entry point
// Parses command line for mode selection: normal, save, load, or silent

#include "Game.h"
#include "GameRecorder.h"
#include <iostream>
#include <cstring>

// Parse command line to determine game mode
GameMode parseCommandLine(int argc, char* argv[]) {
    bool hasLoad = false;
    bool hasSave = false;
    bool hasSilent = false;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        // Convert to lowercase for comparison
        for (char& c : arg) {
            c = static_cast<char>(tolower(c));
        }
        
        if (arg == "-load") hasLoad = true;
        else if (arg == "-save") hasSave = true;
        else if (arg == "-silent") hasSilent = true;
    }
    
    // Determine mode
    if (hasLoad && hasSilent) {
        return GameMode::LoadSilent;
    } else if (hasLoad) {
        return GameMode::Load;
    } else if (hasSave) {
        return GameMode::Save;
    }
    
    return GameMode::Normal;
}

void printUsage() {
    std::cout << "Usage: adv-world.exe [-load|-save] [-silent]\n";
    std::cout << "\n";
    std::cout << "Options:\n";
    std::cout << "  (no args)   Normal gameplay mode (Ex2 behavior)\n";
    std::cout << "  -save       Record gameplay to adv-world.steps and adv-world.result\n";
    std::cout << "  -load       Playback recorded game from files\n";
    std::cout << "  -silent     With -load: run without display, just verify results\n";
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    // Check for help flag
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help" || arg == "-?") {
            printUsage();
            return 0;
        }
    }
    
    // Parse mode from command line
    GameMode mode = parseCommandLine(argc, argv);
    
    // Create game with specified mode
    Game game(mode);
    game.run();
    
    return 0;
}
