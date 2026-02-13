// LoadGameScreen.h - Screen to display and select saved games

#pragma once

#include "BaseScreen.h"
#include "GameStateSaver.h"
#include <vector>
#include <string>

class LoadGameScreen : public BaseScreen {
private:
    std::vector<SaveInfo> saves;
    int selectedIndex;
    ScreenResult result;
    bool rendered;
    std::string selectedFilename;  // Store selected save filename
    
public:
    LoadGameScreen();
    
    void enter() override;
    void handleInput() override;
    void update() override;
    void render() override;
    ScreenResult getResult() const override;
    void reset() override;
    
    // Get the selected save filename (after LoadGame result)
    std::string getSelectedFilename() const;
};

