// MessageDisplay.h - Timed message display

#pragma once

#include <string>

class MessageDisplay {
private:
    std::string message;
    int timer;
    int displayY;
    static bool silentMode;

public:
    MessageDisplay();

    // Setup
    void setDisplayY(int y);
    static void setSilentMode(bool silent);

    // Show message with duration
    void show(const std::string& msg, int duration = 30);

    // Clear immediately
    void clear();

    // Update timer (call once per frame)
    void update();

    // Check if active
    bool isActive() const;

    // Getters
    const std::string& getMessage() const;
    int getTimer() const;

    // Draw
    void draw() const;
};
