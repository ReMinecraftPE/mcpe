/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <vector>

struct MouseInput
{
	int field_0 = 0;
	int field_4 = 0;
	int field_8 = 0;
	int field_C = 0;

	MouseInput() {}
	MouseInput(int x1, int x2, int x3, int x4)
	{
		field_0 = x1;
		field_4 = x2;
		field_8 = x3;
		field_C = x4;
	}
};

class Mouse
{
public:
	static void feed(int, int, int, int);

	// @TODO: There's plenty of inlined code here. Out-line it.

public:
	static std::vector<MouseInput> _inputs;
	static int _index;
	static int _x, _y;
	static int _xOld, _yOld;
	static int _buttonStates[3];
};

