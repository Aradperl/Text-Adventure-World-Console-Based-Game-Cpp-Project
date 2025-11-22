#include <iostream>
#include <windows.h>
#include <conio.h>
#include "Snake.h"
#include "Screen.h"
#include "utils.h"

enum Keys { ESC = 27 };

int main() {
	hideCursor();
	Screen screen;
	screen.draw();
	Snake snakes[] = {
		Snake(Point(10, 10, 1, 0, '@'), "wdxas", screen),
		Snake(Point(15, 5, 0, 1, '*'), "ilmjk", screen)
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
			if (key == Keys::ESC) {
				// Pause - till any key is pressed
				key = _getch();
				if (key == 'H' || key == 'h') {
					break;
				}
			}
			else {
				for (auto& s : snakes) {
					s.handleKeyPressed(key);
				}
			}
		}
		Sleep(50);
	}
	cls();
}