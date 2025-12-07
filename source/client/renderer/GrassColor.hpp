#pragma once

#include <stdint.h>

#include "client/renderer/texture/TextureData.hpp"

class GrassColor
{
public:
	static bool isAvailable() { return _isAvailable; }
	static void setIsAvailable(bool value) { _isAvailable = value; }

	static void init(TextureData& texture);

	static uint32_t get(double x, double y);
	

private:
	static bool _isAvailable;
	static TextureData texture;
};

