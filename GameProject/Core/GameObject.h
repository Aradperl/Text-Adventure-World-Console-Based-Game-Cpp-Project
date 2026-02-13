// GameObject.h - Base class for all game objects

#pragma once

#include "Point.h"
#include "console.h"

class Player;

class GameObject {
protected:
    Point position;
    char symbol;
    Color color;
    bool active;

public:
    // Constructors
    GameObject();
    GameObject(int x, int y, char sym, Color col = Color::White);
    GameObject(const Point& pos, char sym, Color col = Color::White);
    
    virtual ~GameObject() = default;
    
    // Position
    Point getPosition() const;
    int getX() const;
    int getY() const;
    void setPosition(const Point& pos);
    void setPosition(int x, int y);
    
    // Symbol and color
    char getSymbol() const;
    void setSymbol(char sym);
    Color getColor() const;
    void setObjectColor(Color col);
    
    // Active state
    bool isActive() const;
    void setActive(bool state);
    void deactivate();
    void activate();
    
    // Virtual methods (can be overridden)
    virtual void draw() const;
    virtual void erase() const;
    virtual bool collidesWith(const Point& p) const;
    virtual void update();
    virtual bool isBlocking() const;
    virtual bool onInteract(Player& player);
};
