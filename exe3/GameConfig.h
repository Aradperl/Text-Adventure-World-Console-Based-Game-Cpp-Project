// GameConfig.h - Game constants and settings

#pragma once

namespace GameConfig {
    // Console dimensions
    constexpr int SCREEN_WIDTH = 80;
    constexpr int SCREEN_HEIGHT = 25;
    
    // Game area (leave space for status bar)
    constexpr int GAME_WIDTH = 80;
    constexpr int GAME_HEIGHT = 22;
    
    // Timing
    constexpr int GAME_CYCLE_MS = 100;  // milliseconds per cycle
    
    // Game element characters
    constexpr char CHAR_WALL = 'W';
    constexpr char CHAR_PLAYER1 = '$';
    constexpr char CHAR_PLAYER2 = '&';
    constexpr char CHAR_BOMB = '@';
    constexpr char CHAR_KEY = 'K';
    constexpr char CHAR_COIN = 'C';
    constexpr char CHAR_DOOR = 'D';
    constexpr char CHAR_SPRING = '#';
    constexpr char CHAR_EMPTY = ' ';
    constexpr char CHAR_OBSTACLE = '*';
    constexpr char CHAR_TORCH = '!';
    
    // Player 1 controls
    constexpr char P1_UP = 'W';
    constexpr char P1_DOWN = 'X';
    constexpr char P1_LEFT = 'A';
    constexpr char P1_RIGHT = 'D';
    constexpr char P1_STAY = 'S';
    constexpr char P1_DISPOSE = 'E';
    
    // Player 2 controls
    constexpr char P2_UP = 'I';
    constexpr char P2_DOWN = 'M';
    constexpr char P2_LEFT = 'J';
    constexpr char P2_RIGHT = 'L';
    constexpr char P2_STAY = 'K';
    constexpr char P2_DISPOSE = 'O';
    
    // Special keys
    constexpr int KEY_ESC = 27;
}
