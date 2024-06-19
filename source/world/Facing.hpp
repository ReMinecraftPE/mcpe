#pragma once

#include <stdint.h>
#include "compat/LegacyCPPCompatibility.hpp"

class Facing
{
public:
	enum Name
#ifndef USE_OLD_CPP
		: uint8_t // this is compiled as a 32-bit integer in C++03 and earlier
#endif
	{
		DOWN,  // -Y
		UP,    // +Y
		NORTH, // -Z
		SOUTH, // +Z
		WEST,  // -X
		EAST,  // +X
	};
};