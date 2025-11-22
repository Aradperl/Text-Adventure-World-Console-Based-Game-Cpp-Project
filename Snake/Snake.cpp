#include "Snake.h"

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
	body[0].move();
	body[0].draw();
}
