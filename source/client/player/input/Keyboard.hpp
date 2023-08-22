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

#define KEYBOARD_STATES_SIZE (256)

struct KeyboardAction;

class Keyboard
{
public:
	enum KeyState
	{
		UP,
		DOWN
	};

	static void feed(KeyState state, int key);
	static bool next();
	static int  getEventKey();
	static int  getEventKeyState();
	static bool isKeyDown(int keyCode);
	static void reset();

private:
	static std::vector<KeyboardAction> _inputs;
	static Keyboard::KeyState _states[KEYBOARD_STATES_SIZE];
	static int _index;
};

struct KeyboardAction
{
	int _keyState;
	uint8_t _keyCode;

	KeyboardAction(uint8_t keyCode, Keyboard::KeyState keyState)
	{
		_keyState = keyState;
		_keyCode = keyCode;
	}
};
