// MenuScreen.cpp - Main menu with game options

#include "MenuScreen.h"
#include <conio.h>
#include <iostream>

// Constructor
MenuScreen::MenuScreen() 
    : result(ScreenResult::None), hasInput(false), rendered(false) {}

// Enter menu
void MenuScreen::enter() {
    BaseScreen::enter();
    result = ScreenResult::None;
    hasInput = false;
    rendered = false;
}

// Handle input
void MenuScreen::handleInput() {
    if (_kbhit()) {
        char choice = _getch();
        hasInput = true;
        
        switch (choice) {
            case '1':
                result = ScreenResult::StartGame;
                break;
            case '2':
                // Toggle colors
                toggleColorMode();
                rendered = false;  // re-render to show change
                hasInput = false;  // don't exit menu
                break;
            case '3':
                result = ScreenResult::ShowLoadScreen;
                break;
            case '8':
                result = ScreenResult::ShowInstructions;
                break;
            case '9':
                result = ScreenResult::ExitGame;
                break;
            default:
                hasInput = false;
                break;
        }
    }
}

// Update (no dynamic state)
void MenuScreen::update() {
}

// Render menu
void MenuScreen::render() {
    if (rendered) return;
    rendered = true;
    
    clrscr();
    hideCursor();
    
    // Title
    ::setColor(Color::LightCyan);
    gotoxy(25, 5);
    std::cout << "========================================";
    gotoxy(25, 6);
    std::cout << "      TEXT ADVENTURE WORLD GAME        ";
    gotoxy(25, 7);
    std::cout << "========================================";
    
    // Menu options
    resetColor();
    gotoxy(30, 10);
    std::cout << "(1) Start a new game";
    gotoxy(30, 12);
    std::cout << "(2) Toggle Colors: ";
    if (isColorModeEnabled()) {
        ::setColor(Color::LightGreen);
        std::cout << "ON ";
    } else {
        ::setColor(Color::Gray);
        std::cout << "OFF";
    }
    resetColor();
    gotoxy(30, 14);
    std::cout << "(3) Load saved game";
    gotoxy(30, 16);
    std::cout << "(8) Present instructions and keys";
    gotoxy(30, 18);
    std::cout << "(9) EXIT";
    
    ::setColor(Color::Gray);
    gotoxy(25, 20);
    std::cout << "Enter your choice: ";
    resetColor();
    
    std::cout.flush();
}

ScreenResult MenuScreen::getResult() const {
    return hasInput ? result : ScreenResult::None;
}

void MenuScreen::reset() {
    result = ScreenResult::None;
    hasInput = false;
    rendered = false;
}
