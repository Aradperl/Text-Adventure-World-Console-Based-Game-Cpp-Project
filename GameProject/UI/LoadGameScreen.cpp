// LoadGameScreen.cpp - Menu screen for loading saved games

#include "LoadGameScreen.h"
#include "console.h"
#include "GameConfig.h"
#include <conio.h>
#include <iostream>

// Constructor
LoadGameScreen::LoadGameScreen()
    : selectedIndex(0), result(ScreenResult::None), rendered(false) {}

// Enter screen
void LoadGameScreen::enter() {
    BaseScreen::enter();
    result = ScreenResult::None;
    rendered = false;
    selectedIndex = 0;
    selectedFilename.clear();
    
    // Load available saves
    saves = GameStateSaver::getSaveFiles();
}

// Handle input
void LoadGameScreen::handleInput() {
    if (!_kbhit()) return;
    
    char key = _getch();
    
    // Handle special keys (arrows)
    if (key == 0 || key == -32) {
        key = _getch();
        switch (key) {
            case 72:  // Up arrow
                if (!saves.empty() && selectedIndex > 0) {
                    selectedIndex--;
                    rendered = false;
                }
                break;
            case 80:  // Down arrow
                if (!saves.empty() && selectedIndex < (int)saves.size() - 1) {
                    selectedIndex++;
                    rendered = false;
                }
                break;
        }
        return;
    }
    
    // ESC - return to menu
    if (key == GameConfig::KEY_ESC) {
        result = ScreenResult::ExitToMenu;
        return;
    }
    
    // Enter - select save
    if (key == '\r' || key == '\n') {
        if (!saves.empty()) {
            selectedFilename = saves[selectedIndex].filename;
            result = ScreenResult::LoadGame;
        }
        return;
    }
    
    // Number keys 1-9 for quick selection
    if (key >= '1' && key <= '9') {
        int index = key - '1';
        if (index < (int)saves.size()) {
            selectedFilename = saves[index].filename;
            result = ScreenResult::LoadGame;
        }
        return;
    }
    
    // D - Delete selected save
    if (key == 'D' || key == 'd') {
        if (!saves.empty()) {
            GameStateSaver::deleteSave(saves[selectedIndex].filename);
            saves = GameStateSaver::getSaveFiles();  // Refresh list
            if (selectedIndex >= (int)saves.size()) {
                selectedIndex = (int)saves.size() - 1;
            }
            if (selectedIndex < 0) selectedIndex = 0;
            rendered = false;
        }
        return;
    }
}

// Update (no dynamic state)
void LoadGameScreen::update() {
}

// Render screen
void LoadGameScreen::render() {
    if (rendered) return;
    rendered = true;
    
    clrscr();
    hideCursor();
    
    // Title
    ::setColor(Color::LightCyan);
    gotoxy(25, 3);
    std::cout << "========================================";
    gotoxy(25, 4);
    std::cout << "          LOAD SAVED GAME              ";
    gotoxy(25, 5);
    std::cout << "========================================";
    
    resetColor();
    
    if (saves.empty()) {
        // No saves found
        ::setColor(Color::Gray);
        gotoxy(30, 10);
        std::cout << "No saved games found.";
        gotoxy(25, 14);
        std::cout << "Press ESC to return to menu.";
    } else {
        // Instructions
        ::setColor(Color::Gray);
        gotoxy(20, 7);
        std::cout << "Use UP/DOWN arrows or number keys to select";
        gotoxy(20, 8);
        std::cout << "Press ENTER to load, D to delete, ESC to cancel";
        
        // Display saves (max 9 visible)
        int startY = 10;
        int maxVisible = 9;
        int displayCount = (std::min)((int)saves.size(), maxVisible);
        
        for (int i = 0; i < displayCount; i++) {
            gotoxy(20, startY + i);
            
            // Highlight selected
            if (i == selectedIndex) {
                ::setColor(Color::LightGreen);
                std::cout << "> ";
            } else {
                resetColor();
                std::cout << "  ";
            }
            
            // Number for quick select
            std::cout << "(" << (i + 1) << ") ";
            
            // Level and timestamp
            ::setColor(i == selectedIndex ? Color::LightGreen : Color::White);
            std::cout << "Level " << saves[i].level << " - " << saves[i].timestamp;
            
            resetColor();
        }
        
        // Show if there are more saves
        if (saves.size() > (size_t)maxVisible) {
            gotoxy(20, startY + maxVisible);
            ::setColor(Color::Gray);
            std::cout << "... and " << (saves.size() - maxVisible) << " more saves";
        }
    }
    
    resetColor();
    std::cout.flush();
}

ScreenResult LoadGameScreen::getResult() const {
    return result;
}

void LoadGameScreen::reset() {
    result = ScreenResult::None;
    rendered = false;
    selectedIndex = 0;
    selectedFilename.clear();
    saves.clear();
}

std::string LoadGameScreen::getSelectedFilename() const {
    return selectedFilename;
}

