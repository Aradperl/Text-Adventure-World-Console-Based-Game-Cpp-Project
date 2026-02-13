// PauseMenu.h - Pause menu overlay

#pragma once

class PauseMenu {
private:
    bool isPaused;

public:
    PauseMenu();

    // State
    bool getIsPaused() const;
    void pause();
    void resume();
    void toggle();
    void reset();

    // Draw overlay
    void draw() const;
};
