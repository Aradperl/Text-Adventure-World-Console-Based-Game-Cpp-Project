// LightingSystem.h - Dark room visibility

#pragma once

#include "Point.h"
#include "LightSwitch.h"
#include "Torch.h"
#include "Player.h"
#include <vector>
#include <memory>

// Dropped torch
struct DroppedTorch {
    Point position;
    bool collected;
    int lightRadius;

    DroppedTorch(int x, int y, int radius = 5);
};

class LightingSystem {
private:
    bool isDarkRoom;
    int torchRadius;

    // References (not owned)
    const std::vector<std::unique_ptr<LightSwitch>>* lightSwitches;
    const std::vector<std::unique_ptr<Torch>>* torches;
    const std::vector<DroppedTorch>* droppedTorches;
    const Player* player1;
    const Player* player2;

public:
    LightingSystem();

    // Setup
    void setLightSwitches(const std::vector<std::unique_ptr<LightSwitch>>* sw);
    void setTorches(const std::vector<std::unique_ptr<Torch>>* t);
    void setDroppedTorches(const std::vector<DroppedTorch>* dt);
    void setPlayers(const Player* p1, const Player* p2);

    // Settings
    void setDarkRoom(bool dark);
    bool getIsDarkRoom() const;
    void setTorchRadius(int radius);
    int getTorchRadius() const;

    // Checks
    bool isRoomLit() const;
    bool isInTorchLight(int x, int y) const;
    bool isCellVisible(int x, int y) const;
};