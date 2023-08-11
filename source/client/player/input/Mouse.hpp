/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <vector>

struct MouseAction
{
	int field_0;
	int field_4;
	int field_8;
	int field_C;

	MouseAction()
	{
		field_0 = 0;
		field_4 = 0;
		field_8 = 0;
		field_C = 0;
	}
	MouseAction(int x1, int x2, int x3, int x4)
	{
		field_0 = x1;
		field_4 = x2;
		field_8 = x3;
		field_C = x4;
	}

	bool isButton()
	{
		return field_0 == 1 || field_0 == 2;
	}
};

class Mouse
{
public:
	static void feed(int, int, int, int);

	static short getX();
	static short getY();
	static bool  next();
	static bool  isButtonDown(int btn);
	static int   getButtonState(int btn);
	static int   getEventButton();
	static int   getEventButtonState();
	static MouseAction* getEvent();
	static void  setX(int x);
	static void  setY(int y);
	static void  reset();
	static void  reset2();

	// @TODO: There's plenty of inlined code here. Out-line it.

private:
	static std::vector<MouseAction> _inputs;
	static int _index;
	static int _x, _y;
	static int _xOld, _yOld;
	static int _buttonStates[3];
};

