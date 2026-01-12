/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <vector>
#include "world/phys/Vec2.hpp"

class GameControllerManager
{
private:
	static const float DIRECTION_X_THRESHOLD;
	static const float DIRECTION_Y_THRESHOLD;

	static float _deadzonesX[2][2], _deadzonesY[2][2];

public:
	enum StickDirection
	{
		DIR_NONE,
		DIR_UP,
		DIR_RIGHT,
		DIR_DOWN,
		DIR_LEFT
	};

public:
	static bool isValidStick(int stickNo);
	static float linearTransform(float, float, float, bool);
	// SDL2 feeds controller stick update events one axis at a time
	static void feedStickX(int stickNo, bool touched, float x);
	static void feedStickY(int stickNo, bool touched, float y);
	static void feedStick(int stickNo, bool touched, float x, float y);
	static void feedStick(int stickNo, bool touched, const Vec2& vec);
	static float getX(int stickNo);
	static float getY(int stickNo);
	static float getTransformedX(int stickNo, float a2, float a3, bool b);
	static float getTransformedY(int stickNo, float a2, float a3, bool b);
	static StickDirection getXDirection(int stickNo, float deadzone);
	static StickDirection getYDirection(int stickNo, float deadzone);
	static StickDirection getDirection(int stickNo);
	static bool isTouched(int stickNo);
	static bool isValidTrigger(int triggerNo);
	static void feedTrigger(int triggerNo, float x);
	static float getPressure(int triggerNo);
	static void reset();
	static bool isReset() { return inReset; }

public:
	static bool isTouchedValues[2];
	static float stickValuesX[2], stickValuesY[2];
	static float triggerValues[2];
	static bool inReset;
};

