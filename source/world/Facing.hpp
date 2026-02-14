#pragma once

class Facing
{
public:
	enum Name
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
