#pragma once

struct IntRectangle
{
	int x;
	int y;
	int w;
	int h;

	IntRectangle(int x, int y, int width, int height) :
		x(x),
		y(y),
		w(width),
		h(height)
	{

	}

	IntRectangle()
	{
		x = 0;
		y = 0;
		w = 1;
		h = 1;
	}
};
