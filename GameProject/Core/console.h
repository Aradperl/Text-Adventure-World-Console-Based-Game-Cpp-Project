// console.h - Windows console utilities for cursor, colors, and screen
// AI-assisted: Console API wrapper functions

#pragma once

// Disable deprecation warnings for _getch and _kbhit
#pragma warning(disable: 4996)

#include <windows.h>
#include <conio.h>
#include <iostream>

// Move cursor to position (x, y) on console
inline void gotoxy(int x, int y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hConsole, pos);
}

// Clear the console screen
inline void clrscr() {
    system("cls");
}

// Hide the console cursor
inline void hideCursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

// Show the console cursor
inline void showCursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

// Color enum for console text
enum class Color {
    Black = 0,
    Blue = 1,
    Green = 2,
    Cyan = 3,
    Red = 4,
    Magenta = 5,
    Yellow = 6,
    White = 7,
    Gray = 8,
    LightBlue = 9,
    LightGreen = 10,
    LightCyan = 11,
    LightRed = 12,
    LightMagenta = 13,
    LightYellow = 14,
    BrightWhite = 15
};

// Global color mode flag
inline bool& getColorModeEnabled() {
    static bool colorEnabled = true;
    return colorEnabled;
}

// Toggle color mode on/off
inline void toggleColorMode() {
    getColorModeEnabled() = !getColorModeEnabled();
}

// Check if color mode is enabled
inline bool isColorModeEnabled() {
    return getColorModeEnabled();
}

// Set console text color
inline void setColor(Color foreground, Color background = Color::Black) {
    // If colors disabled, do nothing
    if (!getColorModeEnabled()) {
        return;
    }
    
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (int)foreground + ((int)background * 16));
}

// Reset color to default (white on black)
inline void resetColor() {
    // If colors disabled, do nothing
    if (!getColorModeEnabled()) {
        return;
    }
    setColor(Color::White, Color::Black);
}
