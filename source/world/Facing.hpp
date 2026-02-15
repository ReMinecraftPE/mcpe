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
		EAST   // +X
	};
	static const Name OPPOSITE[6];
	static const Name HORIZONTAL[4];
};