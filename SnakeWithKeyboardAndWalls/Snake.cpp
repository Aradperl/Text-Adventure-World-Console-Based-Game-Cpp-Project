#include "Snake.h"

Snake::Snake(const Point& point, const char(&the_keys)[NUM_KEYS + 1], Screen& theScreen)
	: screen(theScreen) 
{
	for (auto& p : body) {
		p = point;
	}
	memcpy(keys, the_keys, NUM_KEYS * sizeof(keys[0]));
}

void Snake::draw() {
	for (auto& p : body) {
		p.draw();
	}
}

void Snake::move() {
	body[SIZE - 1].draw(' ');
	for (size_t index = SIZE - 1; index > 0; --index) {
		body[index] = body[index - 1];
	}
	Point body0orig = body[0];
	body[0].move();
	if (screen.isWall(body[0])) {
		body[0] = body0orig;
	}
	body[0].draw();
}

void Snake::handleKeyPressed(char key_pressed) {
	size_t index = 0;
	for (char k : keys) {
		if (std::tolower(k) == std::tolower(key_pressed)) {
			body[0].setDirection((Direction)index);
			return;
		}
		++index;
	}
}
