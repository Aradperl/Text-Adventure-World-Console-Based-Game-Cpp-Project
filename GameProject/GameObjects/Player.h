// Player.h - Player character

#pragma once

#include "Point.h"
#include "Direction.h"
#include "GameConfig.h"
#include "console.h"
#include "LivesManager.h"

class Player {
private:
    Point position;
    Direction currentDirection;
    char symbol;
    Color color;
    char heldItem;
    bool isMoving;
    int verticalMoveCounter;  // for slower vertical movement
    
    // Spring momentum
    Direction springDirection;
    int springSpeed;
    int springDuration;
    bool hasSpringMomentum;
    
    // Recent movement tracking (for cooperative pushing)
    Direction lastMoveDirection;
    int lastMoveAge;  // cycles since last move (0 = just moved)
    
    // Silent mode (static - affects all players)
    static bool silentMode;
    
public:
    static void setSilentMode(bool silent);
    // Lives (delegates to LivesManager)
    static void addLife();
    static void subtractLife();
    
    Player(int x, int y, char sym, Color col);
    
    // Position
    Point getPosition() const;
    void setPosition(const Point& pos);
    void setPosition(int x, int y);
    
    // Direction
    Direction getDirection() const;
    void setDirection(Direction dir);
    
    // Movement
    bool getIsMoving() const;
    bool canMoveThisFrame();
    void stop();
    Point getNextPosition() const;
    void move();
    
    // Inventory
    char getHeldItem() const;
    void pickupItem(char item);
    char disposeItem();
    bool hasItem() const;
    bool hasKey() const;
    bool hasTorch() const;
    
    // Drawing
    char getSymbol() const;
    Color getColor() const;
    void draw() const;
    void erase() const;
    
    // Spring momentum
    void setSpringMomentum(Direction dir, int speed, int duration);
    bool hasActiveSpringMomentum() const;
    Direction getSpringDirection() const;
    int getSpringSpeed() const;
    void updateSpringMomentum();
    void clearSpringMomentum();
    
    // Force (for pushing obstacles)
    int getForce() const;
    
    // Recent movement (for cooperative pushing)
    Direction getLastMoveDirection() const;
    int getLastMoveAge() const;
    void updateLastMoveAge();  // Call each game cycle
    
    // Bomb
    bool hasBomb() const;
};
