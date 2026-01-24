/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <vector>
#include "world/phys/Vec2.hpp"
#include "GameController.hpp"

class GameControllerManager
{
private:
	static const float DIRECTION_X_THRESHOLD;
	static const float DIRECTION_Y_THRESHOLD;

	static float _deadzonesX[2][2], _deadzonesY[2][2];

public:
	static bool isValidStick(GameController::StickID stickId);
	static float linearTransform(float, float, float, bool);
	// SDL2 feeds controller stick update events one axis at a time
	static void feedStickX(GameController::StickID stickId, bool touched, float x);
	static void feedStickY(GameController::StickID stickId, bool touched, float y);
	static void feedStick(GameController::StickID stickId, bool touched, float x, float y);
	static void feedStick(GameController::StickID stickId, bool touched, const Vec2& vec);
	static float getX(GameController::StickID stickId);
	static float getY(GameController::StickID stickId);
	static float getTransformedX(GameController::StickID stickId, float a2, float a3, bool b);
	static float getTransformedY(GameController::StickID stickId, float a2, float a3, bool b);
	static GameController::StickState getXDirection(GameController::StickID stickId, float deadzone);
	static GameController::StickState getYDirection(GameController::StickID stickId, float deadzone);
	static GameController::StickState getDirection(GameController::StickID stickId);
	static bool isTouched(GameController::StickID stickId);
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

