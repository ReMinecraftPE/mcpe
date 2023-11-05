#pragma once

#include <cstdint>

#include "client/renderer/Texture.hpp"

class GrassColor
{
public:
	static bool isAvailable();

	static void init(Texture texture);

	static uint32_t get(double x, double y);

private:
	static Texture texture;
};

