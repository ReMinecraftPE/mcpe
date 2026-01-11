#pragma once

struct TextureOffset
{
	int x;
	int y;

	TextureOffset()
	{
		x = 0;
		y = 0;
	}

	TextureOffset(int x, int y)
		: x(x)
		, y(y)
	{
	}
};
