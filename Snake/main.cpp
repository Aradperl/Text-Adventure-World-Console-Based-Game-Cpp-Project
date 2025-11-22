#include <iostream>
#include <windows.h>
#include <conio.h>
#include "Snake.h"

enum Keys { ESC = 27 };

int main() {
	hideCursor();
	Snake snakes[] = {
		Snake(10, 10, 1, 0, '@'),
		Snake(15, 5, 0, 1, '#')
	};
	for (auto& s : snakes) {
		s.draw();
	}
	while (true) {
		for(auto& s : snakes) {
			s.move();
		}
		if (_kbhit()) {
			char key = _getch();
			if (key != Keys::ESC) {
				// Pause - till any key is pressed
				key = _getch();
			}
			if (key == Keys::ESC) {
				break;
			}
		}
		Sleep(50);
	}
}