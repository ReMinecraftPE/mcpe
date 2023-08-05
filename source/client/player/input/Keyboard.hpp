/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

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

