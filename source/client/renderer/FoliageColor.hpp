#pragma once

#include <stdint.h>

#include "client/renderer/texture/TextureData.hpp"

class FoliageColor
{
public:
	static bool isAvailable() { return _isAvailable; }
	static void setIsAvailable(bool value) { _isAvailable = value; }

	static void init();

	static uint32_t get(double x, double y);

	static uint32_t getEvergreenColor();

	static uint32_t getBirchColor();

	static uint32_t getDefaultColor();

private:
	static bool _isAvailable;
	static TextureData texture;
};

