/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <vector>
#include "Mouse.hpp"
#include "MouseDevice.hpp"

#define MAX_TOUCHES (8)

struct MouseDevice;
struct MouseAction;

class Multitouch
{
public:
	static int _clampPointerId(int Id);
	static void commit();
	static void feed(MouseButtonType, bool state, int x, int y, int fingerId);
	static MouseDevice* g(int Index);
	static int getActivePointerIds(const int** out);
	static MouseAction* getEvent();
	static int getFirstActivePointerIdEx();
	static int getFirstActivePointerIdExThisUpdate();
	static int getX(int fingerId);
	static int getY(int fingerId);
	static bool isPressed(int fingerId);
	static bool next();
	static void reset();
	static void resetThisUpdate();
	static void rewind();

private:
	static int _activePointerCount;
	static int _activePointerList[MAX_TOUCHES];
	static int _index;
	static bool _wasPressed[MAX_TOUCHES];
	static bool _wasReleased[MAX_TOUCHES];
	static bool _wasPressedThisUpdate[MAX_TOUCHES];
	static bool _wasReleasedThisUpdate[MAX_TOUCHES];
	static MouseDevice _pointers[MAX_TOUCHES];
	static std::vector<MouseAction> _inputs;
};

