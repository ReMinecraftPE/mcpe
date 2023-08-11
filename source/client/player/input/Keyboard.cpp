/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Keyboard.hpp"

#include "GameMods.hpp"

std::vector<KeyboardAction> Keyboard::_inputs;
int Keyboard::_index = -1;
int Keyboard::_states[KEYBOARD_STATES_SIZE];

void Keyboard::feed(int down, int key)
{
#ifndef ORIGINAL_CODE
	// Prevent Crashes
	if (key >= KEYBOARD_STATES_SIZE || key < 0) {
		return;
	}
#endif

	_inputs.push_back(KeyboardAction(key, down));

	_states[key] = down;
}

bool Keyboard::next()
{
	if (_index + 1 >= _inputs.size())
		return false;

	_index++;
	return true;
}

int Keyboard::getEventKey()
{
	return _inputs[_index].field_4;
}

int Keyboard::getEventKeyState()
{
	return _inputs[_index].field_0;
}

bool Keyboard::isKeyDown(int keyCode)
{
	if (keyCode < 0 || keyCode >= KEYBOARD_STATES_SIZE)
		return false;

	return _states[keyCode] == 1;
}

void Keyboard::reset()
{
	_inputs.clear();
	_index = -1;
}
