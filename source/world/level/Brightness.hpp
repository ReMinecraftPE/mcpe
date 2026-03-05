#pragma once
#include <cstdint>

// int8 to prevent overflow issues
typedef int8_t Brightness_t;

class Brightness
{
public:
	static constexpr Brightness_t MIN = 0;
	static constexpr Brightness_t MAX = 15;
};
