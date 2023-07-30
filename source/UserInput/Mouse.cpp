/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Mouse.hpp"

std::vector<MouseInput> Mouse::_inputs;
int Mouse::_index, Mouse::_x, Mouse::_y;
int Mouse::_xOld, Mouse::_yOld, Mouse::_buttonStates[3];

void Mouse::feed(int x1, int x2, int x3, int x4)
{
	_inputs.push_back(MouseInput(x1, x2, x3, x4));

	if (x1)
		Mouse::_buttonStates[x1] = x2;

	_xOld = _x;
	_yOld = _y;
	_x = x3;
	_y = x4;
}
