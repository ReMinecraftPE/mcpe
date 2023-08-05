/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Keyboard.hpp"

#include "GameMods.hpp"

std::vector<Keyboard::Input> Keyboard::_inputs;
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

	Input i;
	i.field_0 = down;
	i.field_4 = uint8_t(key);
	_inputs.push_back(i);

	_states[key] = down;
}
