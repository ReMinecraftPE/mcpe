/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Controller.hpp"
#include <cmath>

bool Controller::isTouchedValues[2];
float Controller::stickValuesX[2];
float Controller::stickValuesY[2];
float Controller::triggerValues[2];
bool Controller::inReset;

#ifndef USE_NATIVE_ANDROID
const float Controller_unk_1[3] = { 0.0f, 0.64f, -0.64f };
#endif

bool Controller::isValidStick(int stickNo)
{
	// We have 2 'sticks' on the Xperia Play
	return stickNo >= 1 && stickNo <= 2;
}

float Controller::linearTransform(float a1, float a2, float a3, bool b)
{
	if (a1 < 0.0)
		a2 = -a2;
	if (fabsf(a2) >= fabsf(a1))
		return 0.0;

	float v5 = (a1 - a2) * a3;
	if (!b || fabsf(v5) <= 1.0)
		return v5;
	if (v5 > 0.0)
		return 1.0;
	return -1.0;
}

void Controller::feedStickX(int stickNo, int touched, float x)
{
	if (!isValidStick(stickNo))
		return;

	int index = stickNo - 1;

	isTouchedValues[index] = touched != 0;
	stickValuesX[index] = x;

	inReset = false;
}

void Controller::feedStickY(int stickNo, int touched, float y)
{
	if (!isValidStick(stickNo))
		return;

	int index = stickNo - 1;

	isTouchedValues[index] = touched != 0;
	stickValuesY[index] = y;

	inReset = false;
}

void Controller::feedStick(int stickNo, int touched, float x, float y)
{
	if (!isValidStick(stickNo))
		return;

#ifdef USE_NATIVE_ANDROID
	int index = stickNo - 1;
#else
	int index = (x >= 0.0f) ? 1 : 0;
#endif

	// maybe the 2 'touch sticks' are actually internally 1 single surface??

	isTouchedValues[index] = touched != 0;
#ifdef USE_NATIVE_ANDROID
	stickValuesX[index] = x;
#else
	stickValuesX[index] = linearTransform(x + Controller_unk_1[index + 1], 0.0f, 2.78f, true);
#endif

	stickValuesY[index] = y;

	inReset = false;
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
			return Controller::getXDirection(stickNo, 0.3f);
		else
			return Controller::getYDirection(stickNo, 0.3f);
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
