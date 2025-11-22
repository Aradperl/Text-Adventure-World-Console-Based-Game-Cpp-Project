#pragma once

#include "Point.h"

class Snake {
	static constexpr int SIZE = 5;
	Point body[SIZE];
public:
	Snake(int x, int y, int diffx, int diffy, char c) {
		for (auto& p : body) {
			p = Point(x, y, diffx, diffy, c);
		}
	}
	void draw();
	void move();
};
