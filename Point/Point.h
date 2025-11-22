#pragma once

#include <iostream>
#include "utils.h"

class Point {
	int x, y;
	int diff_x, diff_y;
	char ch;
	enum {MAX_X = 80, MAX_Y = 25};
public:
	Point(int x1, int y1, int diffx, int diffy, char c) {
		x = x1;
		y = y1;
		diff_x = diffx;
		diff_y = diffy;
		ch = c;
	}
	void draw() {
		draw(ch);
	}
	void draw(char c) {
		gotoxy(x, y);
		std::cout << c;
	}
	void move();
};

