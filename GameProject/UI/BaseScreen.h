// BaseScreen.h - Abstract base class for all game screens

#pragma once

#include "console.h"
#include "GameConfig.h"

class Game;

// Enum for screen transition results
enum class ScreenResult {
    None,
    StartGame,
    ShowInstructions,
    ShowLoadScreen,
    LoadGame,
    NextScreen,
    ExitToMenu,
    ExitGame,
    GameOver
};

// Base class for all screens in the game
class BaseScreen {
protected:
    bool active;
    
public:
    BaseScreen();
    virtual ~BaseScreen() = default;
    
    // Core screen lifecycle methods
    virtual void init();
    virtual void enter();
    virtual void exit();
    virtual void reset();
    
    // Main screen methods (pure virtual)
    virtual void handleInput() = 0;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual ScreenResult getResult() const = 0;
    
    // Check if screen is active
    bool isActive() const;
};
