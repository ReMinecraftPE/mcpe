/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Controller.hpp"
#include <cmath>

const float Controller::DIRECTION_X_THRESHOLD = 0.3f;
const float Controller::DIRECTION_Y_THRESHOLD = 0.3f;

#define DEADZONE(d) { (d), (1.0f / (1.0f - d)) }
// 0.3051f
float Controller::_deadzonesX[][2] = { DEADZONE(0.3f), DEADZONE(0.3f) }; // Java: { DEADZONE(0.15f), DEADZONE(0.20f) };
float Controller::_deadzonesY[][2] = { DEADZONE(0.3f), DEADZONE(0.3f) }; // Java: { DEADZONE(0.15f), DEADZONE(0.20f) };

bool Controller::isTouchedValues[] = { false, false };
float Controller::stickValuesX[] = { 0.0f, 0.0f };
float Controller::stickValuesY[] = { 0.0f, 0.0f };
float Controller::triggerValues[] = { 0.0f, 0.0f };
bool Controller::inReset = true;

bool Controller::isValidStick(int stickNo)
{
	// We have 2 'sticks' on the Xperia Play
	return stickNo >= 1 && stickNo <= 2;
}

float Controller::linearTransform(float a1, float a2, float a3, bool b)
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

void Controller::feedStickX(int stickNo, bool touched, float x)
{
	if (!isValidStick(stickNo))
		return;

	int index = stickNo - 1;
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

void Controller::feedStickY(int stickNo, bool touched, float y)
{
	if (!isValidStick(stickNo))
		return;

	int index = stickNo - 1;
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

void Controller::feedStick(int stickNo, bool touched, float x, float y)
{
	feedStickX(stickNo, touched, x);
	feedStickY(stickNo, touched, y);
}

float Controller::getX(int stickNo)
{
	if (!isValidStick(stickNo))
		return 0.0f;

	return stickValuesX[stickNo - 1];
}

float Controller::getY(int stickNo)
{
	if (!isValidStick(stickNo))
		return 0.0f;

	return stickValuesY[stickNo - 1];
}

float Controller::getTransformedX(int stickNo, float a2, float a3, bool b)
{
	if (!isValidStick(stickNo))
		return 0.0f;

	// @BUG: subtracting by 1? What if we're trying to grab stick 0?
	return linearTransform(stickValuesX[stickNo - 1], a2, a3, b);
}

float Controller::getTransformedY(int stickNo, float a2, float a3, bool b)
{
	if (!isValidStick(stickNo))
		return 0.0f;

	// @BUG: subtracting by 1? What if we're trying to grab stick 0?
	return linearTransform(stickValuesY[stickNo - 1], a2, a3, b);
}

Controller::StickDirection Controller::getXDirection(int stickNo, float deadzone)
{
	if (Controller::isValidStick(stickNo) && Controller::isTouched(stickNo))
	{
		float x = Controller::getX(stickNo);
		if (x >= deadzone)  return DIR_RIGHT;
		if (x <= -deadzone) return DIR_LEFT;
	}
	return DIR_NONE;
}

Controller::StickDirection Controller::getYDirection(int stickNo, float deadzone)
{
	if (Controller::isValidStick(stickNo) && Controller::isTouched(stickNo))
	{
		float y = Controller::getY(stickNo);
		if (y >= deadzone)  return DIR_UP;
		if (y <= -deadzone) return DIR_DOWN;
	}
	return DIR_NONE;
}

Controller::StickDirection Controller::getDirection(int stickNo)
{
	if (Controller::isValidStick(stickNo) && Controller::isTouched(stickNo))
	{
		float x = fabsf(Controller::getX(stickNo));
		float y = fabsf(Controller::getY(stickNo));
		if (x >= y)
			return Controller::getXDirection(stickNo, DIRECTION_X_THRESHOLD);
		else
			return Controller::getYDirection(stickNo, DIRECTION_Y_THRESHOLD);
	}

	return DIR_NONE;
}

bool Controller::isTouched(int stickNo)
{
	if (!isValidStick(stickNo))
		return false;

	return isTouchedValues[stickNo - 1];
}

bool Controller::isValidTrigger(int triggerNo)
{
	// Most controllers have 2 triggers
	return triggerNo >= 1 && triggerNo <= 2;
}

void Controller::feedTrigger(int triggerNo, float x)
{
	triggerValues[triggerNo - 1] = x;
	inReset = false;
}

float Controller::getPressure(int triggerNo)
{
	return isValidTrigger(triggerNo) ? triggerValues[triggerNo - 1] : 0;
}

void Controller::reset()
{
	feedStick(1, 0, 0.0f, 0.0f);
	feedStick(2, 0, 0.0f, 0.0f);
	feedTrigger(1, 0.0f);
	feedTrigger(2, 0.0f);
	inReset = true;
}
