#pragma once

class DyeColor
{
public:
	enum Name 
	{
		BLACK, RED, GREEN, BROWN, BLUE, PURPLE, CYAN, SILVER, GRAY, PINK, LIME, YELLOW, LIGHT_BLUE, MAGENTA, ORANGE, WHITE
	};

	static const char* IDS[];
	static const int RGBS[];
};
