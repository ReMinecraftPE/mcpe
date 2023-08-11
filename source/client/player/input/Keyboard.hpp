/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once
#if __cplusplus < 201103L
#include <stdint.h>
#endif

#include <vector>
#include <cstdint>

#define KEYBOARD_STATES_SIZE 256

struct KeyboardAction
{
	int field_0;
	uint8_t field_4;

	KeyboardAction(uint8_t key, int state)
	{
		field_0 = state;
		field_4 = key;
	}
};

class Keyboard
{
public:
	static void feed(int down, int key);
	static bool next();
	static int  getEventKey();
	static int  getEventKeyState();
	static bool isKeyDown(int keyCode);
	static void reset();

private:
	static std::vector<KeyboardAction> _inputs;
	static int _states[KEYBOARD_STATES_SIZE];
	static int _index;
};

