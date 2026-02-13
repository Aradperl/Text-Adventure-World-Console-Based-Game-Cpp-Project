// GameObject.cpp - Base class for all game objects

#include "GameObject.h"
#include <iostream>

// Constructors
GameObject::GameObject() 
    : position(0, 0), symbol(' '), color(Color::White), active(true) {}

GameObject::GameObject(int x, int y, char sym, Color col)
    : position(x, y), symbol(sym), color(col), active(true) {}

GameObject::GameObject(const Point& pos, char sym, Color col)
    : position(pos), symbol(sym), color(col), active(true) {}

// Position stuff
Point GameObject::getPosition() const { return position; }
int GameObject::getX() const { return position.getX(); }
int GameObject::getY() const { return position.getY(); }
void GameObject::setPosition(const Point& pos) { position = pos; }
void GameObject::setPosition(int x, int y) { position.set(x, y); }

// Symbol and color
char GameObject::getSymbol() const { return symbol; }
void GameObject::setSymbol(char sym) { symbol = sym; }
Color GameObject::getColor() const { return color; }
void GameObject::setObjectColor(Color col) { color = col; }

// Active state
bool GameObject::isActive() const { return active; }
void GameObject::setActive(bool state) { active = state; }
void GameObject::deactivate() { active = false; }
void GameObject::activate() { active = true; }

// Draw object on screen
void GameObject::draw() const {
    if (active) {
        gotoxy(position.getX(), position.getY());
        ::setColor(color);
        std::cout << symbol;
        resetColor();
    }
}

// Erase from screen
void GameObject::erase() const {
    gotoxy(position.getX(), position.getY());
    std::cout << ' ';
}

// Check collision with point
bool GameObject::collidesWith(const Point& p) const {
    return active && (position == p);
}

// Update (can be overridden)
void GameObject::update() {}

// Blocking behavior (can be overridden)
bool GameObject::isBlocking() const { return false; }

// Interaction with player (can be overridden)
bool GameObject::onInteract(Player& player) { return false; }
