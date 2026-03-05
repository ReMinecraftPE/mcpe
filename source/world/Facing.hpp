#pragma once

struct TilePos;

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
		COUNT
	};

public:
	static Name DIRECTIONS[COUNT];
	static TilePos DIRECTION[COUNT];
};
