/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <stddef.h>
#include "Keyboard.hpp"

#include "GameMods.hpp"

std::vector<KeyboardAction> Keyboard::_inputs;
int Keyboard::_index = -1;
Keyboard::KeyState Keyboard::_states[KEYBOARD_STATES_SIZE];

void Keyboard::feed(KeyState state, int key)
{
	// Prevent Crashes
	if (key >= KEYBOARD_STATES_SIZE || key < 0)
		return;

	_inputs.push_back(KeyboardAction(key, state));

	_states[key] = state;
}

bool Keyboard::next()
{
	if ((size_t)_index + 1 >= _inputs.size())
		return false;

	_index++;
	return true;
}

int Keyboard::getEventKey()
{
	return _inputs[_index]._keyCode;
}

int Keyboard::getEventKeyState()
{
	return _inputs[_index]._keyState;
}

bool Keyboard::isKeyDown(int keyCode)
{
	if (keyCode < 0 || keyCode >= KEYBOARD_STATES_SIZE)
		return false;

	return _states[keyCode] == DOWN;
}

void Keyboard::reset()
{
	_inputs.clear();
	_index = -1;
}
