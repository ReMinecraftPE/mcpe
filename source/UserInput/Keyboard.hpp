#pragma once

#include <vector>

class Keyboard
{
public:
	//@TODO: Rename this to KeyboardAction
	struct Input
	{
		int field_0;
		uint8_t field_4;
	};

	static std::vector<Input> _inputs;
	static int _states[256];
	static int _index;

	// likely inlined
	static void feed(int down, int key);
};

