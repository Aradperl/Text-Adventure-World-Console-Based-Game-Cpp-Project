#pragma once

#include "Point.h"
#include "Screen.h"

class Snake {
	static constexpr int SIZE = 5;
	static constexpr int NUM_KEYS = 5;
	Point body[SIZE];
	char keys[NUM_KEYS];
	Screen& screen;
public:
	// ctor gets start point and an array of 5 keys
	Snake(const Point& point, const char(&the_keys)[NUM_KEYS + 1], Screen& theScreen);
	void draw();
	void move();
	void handleKeyPressed(char key);
};
