#pragma once
#include <stdint.h>

class Facing
{
public:
	enum Name : uint8_t
	{
		DOWN,  // -Y
		UP,    // +Y
		NORTH, // -Z
		SOUTH, // +Z
		WEST,  // -X
		EAST,  // +X
		MAX
	};

public:
	static Name DIRECTIONS[MAX];
};
