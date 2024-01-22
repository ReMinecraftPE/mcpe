#pragma once

#include <stdint.h>

#include "client/renderer/Texture.hpp"

class FoliageColor
{
public:
	static bool isAvailable();

	static void init(Texture texture);

	static uint32_t get(double x, double y);

	static uint32_t getEvergreenColor();

	static uint32_t getBirchColor();

	static uint32_t getDefaultColor();

private:
	static Texture texture;
};

