#pragma once

#include <stdint.h>

struct MenuPointer
{
	typedef float Unit;

	Unit x;
	Unit y;
	bool isPressed;

	MenuPointer()
	{
		x = 0;
		y = 0;
		isPressed = false;
	}

	MenuPointer(Unit x, Unit y, bool isPressed = true)
		: x(x)
		, y(y)
		, isPressed(isPressed)
	{
	}
};

