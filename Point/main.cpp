#include <iostream>
#include <windows.h>
#include <conio.h>
#include "Point.h"

enum Keys { ESC = 27 };

int main() {
	hideCursor();
	Point points[] = {
		Point(10, 10, 1, 1, '@'),
		Point(15, 10, 1, 0, '#'),
		Point(10, 15, 0, 1, '*')
	};
	while (true) {
		for(auto& p : points) {
			p.draw(' ');
			p.move();
			p.draw();
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