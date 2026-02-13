// Switch.h - On/Off switch that can be linked to gates

#pragma once

#include "GameObject.h"
#include "GameConfig.h"

class Player;

class Switch : public GameObject {
private:
    bool isOn;
    char linkedGate;  // Which gate this switch is linked to (by door number)

public:
    Switch();
    Switch(int x, int y, char gateNum, bool startOn = false);
    Switch(const Point& pos, char gateNum, bool startOn = false);

    // State
    bool getIsOn() const;
    void toggle();
    void turnOn();
    void turnOff();
    void reset();

    // Gate link
    char getLinkedGate() const;

    // Interaction
    bool onInteract(Player& player) override;

    // Draw
    void draw() const override;
};