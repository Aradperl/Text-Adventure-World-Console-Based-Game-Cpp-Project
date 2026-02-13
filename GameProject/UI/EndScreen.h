// EndScreen.h - Victory/End game screen

#pragma once

#include "BaseScreen.h"

// Screen displayed when players win the game
class EndScreen : public BaseScreen {
private:
    bool waitingForKey;
    ScreenResult result;
    bool rendered;

public:
    EndScreen();
    
    void enter() override;
    void handleInput() override;
    void update() override;
    void render() override;
    ScreenResult getResult() const override;
    void reset() override;
};
