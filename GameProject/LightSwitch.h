// LightSwitch.h - Light switch object

#pragma once

#include "GameObject.h"
#include "GameConfig.h"

class Player;

class LightSwitch : public GameObject {
private:
    bool isOn;

public:
    LightSwitch();
    LightSwitch(int x, int y, bool startOn = false);
    LightSwitch(const Point& pos, bool startOn = false);

    // State
    bool getIsOn() const;
    void toggle();
    void turnOn();
    void turnOff();
    void reset();

    // Interaction
    bool onInteract(Player& player) override;

    // Draw
    void draw() const override;
};