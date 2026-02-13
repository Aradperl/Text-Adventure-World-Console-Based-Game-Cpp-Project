// MenuScreen.h - Main menu screen

#pragma once

#include "BaseScreen.h"

class MenuScreen : public BaseScreen {
private:
    ScreenResult result;
    bool hasInput;
    bool rendered;

public:
    MenuScreen();
    
    void enter() override;
    void handleInput() override;
    void update() override;
    void render() override;
    ScreenResult getResult() const override;
    void reset() override;
};
