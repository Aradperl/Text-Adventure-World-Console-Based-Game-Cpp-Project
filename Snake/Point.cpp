#include "Point.h"

void Point::move()
{
	x = (x + diff_x) % MAX_X;
	y = (y + diff_y) % MAX_Y;
}
