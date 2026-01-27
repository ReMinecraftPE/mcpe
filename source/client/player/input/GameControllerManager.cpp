/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <cmath>

#include "GameControllerManager.hpp"

const float GameControllerManager::DIRECTION_X_THRESHOLD = 0.3f;
const float GameControllerManager::DIRECTION_Y_THRESHOLD = 0.3f;

#define DEADZONE(d) { (d), (1.0f / (1.0f - d)) }
// 0.3051f
float GameControllerManager::_deadzonesX[][2] = { DEADZONE(0.3f), DEADZONE(0.3f) }; // Java: { DEADZONE(0.15f), DEADZONE(0.20f) };
float GameControllerManager::_deadzonesY[][2] = { DEADZONE(0.3f), DEADZONE(0.3f) }; // Java: { DEADZONE(0.15f), DEADZONE(0.20f) };

bool GameControllerManager::isTouchedValues[] = { false, false };
float GameControllerManager::stickValuesX[] = { 0.0f, 0.0f };
float GameControllerManager::stickValuesY[] = { 0.0f, 0.0f };
float GameControllerManager::triggerValues[] = { 0.0f, 0.0f };
bool GameControllerManager::inReset = true;

bool GameControllerManager::isValidStick(GameController::StickID stickId)
{
	// We have 2 'sticks' on the Xperia Play
	return stickId >= 1 && stickId <= 2;
}

float GameControllerManager::linearTransform(float a1, float a2, float a3, bool b)
{
	if (a1 < 0.0f)
		a2 = -a2;
	if (fabsf(a2) >= fabsf(a1))
		return 0.0f;

	float v5 = (a1 - fabsf(a2)) * a3;
	if (!b || fabsf(v5) <= 1.0f)
		return v5;
	if (v5 > 0.0f)
		return 1.0f;
	return -1.0f;
}

void GameControllerManager::feedStickX(GameController::StickID stickId, bool touched, float x)
{
	if (!isValidStick(stickId))
		return;

	int index = stickId - 1;
	float deadzone = _deadzonesX[index][0], deadzone_mod = _deadzonesX[index][1];

	isTouchedValues[index] = touched;
	if (x >= deadzone)
		x = (x - deadzone) * deadzone_mod;
	else if (x <= -deadzone)
		x = (x + deadzone) * deadzone_mod;
	else
		x = 0;
	stickValuesX[index] = x; // LCE has a deadzone of 20000, ~0.3f on 360

	inReset = false;
}

void GameControllerManager::feedStickY(GameController::StickID stickId, bool touched, float y)
{
	if (!isValidStick(stickId))
		return;

	int index = stickId - 1;
	float deadzone = _deadzonesY[index][0], deadzone_mod = _deadzonesY[index][1];

	isTouchedValues[index] = touched;
	if (y >= deadzone)
		y = (y - deadzone) * deadzone_mod;
	else if (y <= -deadzone)
		y = (y + deadzone) * deadzone_mod;
	else
		y = 0;
	stickValuesY[index] = y; // LCE has a deadzone of 20000, ~0.3f on 360

	inReset = false;
}

void GameControllerManager::feedStick(GameController::StickID stickId, bool touched, float x, float y)
{
	feedStickX(stickId, touched, x);
	feedStickY(stickId, touched, y);
}

void GameControllerManager::feedStick(GameController::StickID stickId, bool touched, const Vec2& vec)
{
	feedStick(stickId, touched, vec.x, vec.y);
}

float GameControllerManager::getX(GameController::StickID stickId)
{
	if (!isValidStick(stickId))
		return 0.0f;

	return stickValuesX[stickId - 1];
}

float GameControllerManager::getY(GameController::StickID stickId)
{
	if (!isValidStick(stickId))
		return 0.0f;

	return stickValuesY[stickId - 1];
}

float GameControllerManager::getTransformedX(GameController::StickID stickId, float a2, float a3, bool b)
{
	if (!isValidStick(stickId))
		return 0.0f;

	// @BUG: subtracting by 1? What if we're trying to grab stick 0?
	return linearTransform(stickValuesX[stickId - 1], a2, a3, b);
}

float GameControllerManager::getTransformedY(GameController::StickID stickId, float a2, float a3, bool b)
{
	if (!isValidStick(stickId))
		return 0.0f;

	// @BUG: subtracting by 1? What if we're trying to grab stick 0?
	return linearTransform(stickValuesY[stickId - 1], a2, a3, b);
}

GameController::StickState GameControllerManager::getXDirection(GameController::StickID stickId, float deadzone)
{
	if (GameControllerManager::isValidStick(stickId) && GameControllerManager::isTouched(stickId))
	{
		float x = GameControllerManager::getX(stickId);
		if (x >= deadzone)  return GameController::STICK_STATE_RIGHT;
		if (x <= -deadzone) return GameController::STICK_STATE_LEFT;
	}
	return GameController::STICK_STATE_NONE;
}

GameController::StickState GameControllerManager::getYDirection(GameController::StickID stickId, float deadzone)
{
	if (GameControllerManager::isValidStick(stickId) && GameControllerManager::isTouched(stickId))
	{
		float y = GameControllerManager::getY(stickId);
		if (y >= deadzone)  return GameController::STICK_STATE_UP;
		if (y <= -deadzone) return GameController::STICK_STATE_DOWN;
	}
	return GameController::STICK_STATE_NONE;
}

GameController::StickState GameControllerManager::getDirection(GameController::StickID stickId)
{
	if (GameControllerManager::isValidStick(stickId) && GameControllerManager::isTouched(stickId))
	{
		float x = fabsf(GameControllerManager::getX(stickId));
		float y = fabsf(GameControllerManager::getY(stickId));
		if (x >= y)
			return GameControllerManager::getXDirection(stickId, DIRECTION_X_THRESHOLD);
		else
			return GameControllerManager::getYDirection(stickId, DIRECTION_Y_THRESHOLD);
	}

	return GameController::STICK_STATE_NONE;
}

bool GameControllerManager::isTouched(GameController::StickID stickId)
{
	if (!isValidStick(stickId))
		return false;

	return isTouchedValues[stickId - 1];
}

bool GameControllerManager::isValidTrigger(int triggerNo)
{
	// Most controllers have 2 triggers
	return triggerNo >= 1 && triggerNo <= 2;
}

void GameControllerManager::feedTrigger(int triggerNo, float x)
{
	triggerValues[triggerNo - 1] = x;
	inReset = false;
}

float GameControllerManager::getPressure(int triggerNo)
{
	return isValidTrigger(triggerNo) ? triggerValues[triggerNo - 1] : 0;
}

void GameControllerManager::reset()
{
	feedStick(1, 0, 0.0f, 0.0f);
	feedStick(2, 0, 0.0f, 0.0f);
	feedTrigger(1, 0.0f);
	feedTrigger(2, 0.0f);
	inReset = true;
}
