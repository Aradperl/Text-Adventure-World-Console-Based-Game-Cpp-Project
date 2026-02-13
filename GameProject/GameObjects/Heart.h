// Heart.h - Heart pickup that adds a life

#pragma once

#include "GameObject.h"

class Heart : public GameObject {
private:
    int spawnTimer;
    static const int SPAWN_DURATION = 50;  // about 5 seconds

public:
    Heart();
    Heart(int x, int y);
    
    // Timer stuff
    void resetTimer();
    bool updateTimer();  // returns true if expired
    int getTimeRemaining() const;
    
    bool isBlocking() const override;
};
