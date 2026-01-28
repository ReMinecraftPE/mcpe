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
	
	static bool isHorizontal(Name face)
	{
		return face != DOWN && face != UP;
	}
	
	static bool isVertical(Name face)
	{
		return face == DOWN || face == UP;
	}
};
