// EndScreen.cpp - Victory screen displayed after completing all levels

#include "EndScreen.h"
#include <conio.h>
#include <iostream>

// Constructor
EndScreen::EndScreen() 
    : waitingForKey(true), result(ScreenResult::None), rendered(false) {}

// Enter screen and initialize state
void EndScreen::enter() {
    BaseScreen::enter();
    waitingForKey = true;
    result = ScreenResult::None;
    rendered = false;
}

// Handle keyboard input
void EndScreen::handleInput() {
    if (waitingForKey && _kbhit()) {
        _getch();
        result = ScreenResult::ExitToMenu;
        waitingForKey = false;
    }
}

// Update screen state (no dynamic state for end screen)
void EndScreen::update() {
    // End screen has no dynamic state
}

// Render the victory screen
void EndScreen::render() {
    if (rendered) return;
    rendered = true;
    
    clrscr();
    
    // Victory message
    ::setColor(Color::LightYellow);
    gotoxy(25, 8);
    std::cout << "========================================";
    gotoxy(25, 9);
    std::cout << "      CONGRATULATIONS! YOU WON!        ";
    gotoxy(25, 10);
    std::cout << "========================================";
    
    ::setColor(Color::LightGreen);
    gotoxy(25, 13);
    std::cout << "Both players worked together and";
    gotoxy(25, 14);
    std::cout << "completed the Text Adventure World!";
    
    // Instructions
    resetColor();
    gotoxy(25, 18);
    std::cout << "Press any key to return to menu...";
    
    std::cout.flush();
}

// Get current screen result
ScreenResult EndScreen::getResult() const {
    return result;
}

// Reset screen to initial state
void EndScreen::reset() {
    waitingForKey = true;
    result = ScreenResult::None;
    rendered = false;
}
