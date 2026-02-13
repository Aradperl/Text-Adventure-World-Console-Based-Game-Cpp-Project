// LightingSystem.cpp - Dark room visibility calculations
// Handles torch light radius, light switches, and cell visibility
// AI usage: Distance-squared optimization for torch light (AI-assisted)

#include "LightingSystem.h"

DroppedTorch::DroppedTorch(int x, int y, int radius)
    : position(x, y), collected(false), lightRadius(radius) {
}

LightingSystem::LightingSystem()
    : isDarkRoom(false), torchRadius(3),
    lightSwitches(nullptr), torches(nullptr), droppedTorches(nullptr),
    player1(nullptr), player2(nullptr) {
}

// Dependency injection
void LightingSystem::setLightSwitches(const std::vector<std::unique_ptr<LightSwitch>>* sw) { lightSwitches = sw; }
void LightingSystem::setTorches(const std::vector<std::unique_ptr<Torch>>* t) { torches = t; }
void LightingSystem::setDroppedTorches(const std::vector<DroppedTorch>* dt) { droppedTorches = dt; }
void LightingSystem::setPlayers(const Player* p1, const Player* p2) { player1 = p1; player2 = p2; }

void LightingSystem::setDarkRoom(bool dark) { isDarkRoom = dark; }
bool LightingSystem::getIsDarkRoom() const { return isDarkRoom; }
void LightingSystem::setTorchRadius(int radius) { torchRadius = radius; }
int LightingSystem::getTorchRadius() const { return torchRadius; }

// Check if room is lit by any light switch
bool LightingSystem::isRoomLit() const {
    if (!isDarkRoom) return true;

    if (lightSwitches) {
        for (const auto& sw : *lightSwitches) {
            if (sw->isActive() && sw->getIsOn()) {
                return true;
            }
        }
    }
    return false;
}

// AI-assisted: Check if cell is lit by any torch (uses distance^2 to avoid sqrt)
bool LightingSystem::isInTorchLight(int x, int y) const {
    Point p(x, y);

    // Player 1's held torch
    if (player1 && player1->hasTorch()) {
        int dx = x - player1->getPosition().getX();
        int dy = y - player1->getPosition().getY();
        if ((dx * dx + dy * dy) <= (torchRadius * torchRadius)) {
            return true;
        }
    }

    // Player 2's held torch
    if (player2 && player2->hasTorch()) {
        int dx = x - player2->getPosition().getX();
        int dy = y - player2->getPosition().getY();
        if ((dx * dx + dy * dy) <= (torchRadius * torchRadius)) {
            return true;
        }
    }

    // Placed torches
    if (torches) {
        for (const auto& torch : *torches) {
            if (torch->isActive() && !torch->isCollected()) {
                if (torch->isInLightRadius(p)) {
                    return true;
                }
            }
        }
    }

    // Dropped torches
    if (droppedTorches) {
        for (const auto& dt : *droppedTorches) {
            if (!dt.collected) {
                int dx = x - dt.position.getX();
                int dy = y - dt.position.getY();
                if ((dx * dx + dy * dy) <= (dt.lightRadius * dt.lightRadius)) {
                    return true;
                }
            }
        }
    }

    return false;
}

// Determine if cell should be rendered (lit room, torch light, or not dark)
bool LightingSystem::isCellVisible(int x, int y) const {
    if (!isDarkRoom) return true;
    if (isRoomLit()) return true;
    return isInTorchLight(x, y);
}
