// InstructionsScreen.h - Help screen

#pragma once

#include "BaseScreen.h"

class InstructionsScreen : public BaseScreen {
private:
    bool waitingForKey;
    ScreenResult result;
    bool rendered;

public:
    InstructionsScreen();
    
    void enter() override;
    void handleInput() override;
    void update() override;
    void render() override;
    ScreenResult getResult() const override;
    void reset() override;
};
