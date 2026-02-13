// Door.h - Door game object

#pragma once

#include "GameObject.h"
#include "GameConfig.h"

class Player;

// Door object that requires keys to open
class Door : public GameObject {
private:
    char doorNum;
    int keysRequired;
    int keysUsed;
    bool isOpen;
    int targetScreen;

public:
    // Constructors
    Door();
    Door(int x, int y, char num, int keys = 1, int target = -1);
    Door(const Point& pos, char num, int keys = 1, int target = -1);
    
    // Door properties
    char getDoorNum() const;
    int getKeysRequired() const;
    int getKeysUsed() const;
    int getTargetScreen() const;
    bool isDoorOpen() const;
    
    // Set target screen
    void setTargetScreen(int screen);
    
    // Try to use a key
    bool useKey();
    
    // Open the door
    void open();
    
    // Reset
    void reset();
    
    // Blocking
    bool isBlocking() const override;
    
    // Player interaction
    bool onInteract(Player& player) override;
    
    // Drawing
    void draw() const override;
};
