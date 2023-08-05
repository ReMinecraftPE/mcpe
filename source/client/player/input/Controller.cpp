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
const float Controller_unk_1[3] = { 0.0f, 0.64f, -0.64f };

bool Controller::isValidStick(int stickNo)
{
	// We have 2 'sticks' on the Xperia Play
	return stickNo >= 1 && stickNo <= 2;
}

float Controller::linearTransform(float a1, float a2, float a3, bool b)
{
	float x1 = fabsf(a2);
	float x2 = a1;
	float x3 = fabsf(x1);
	if (x3 >= x2)
		return 0.0f;

	float x4 = (a1 - x1) * a3;
	if (b)
	{
		float x5 = fabsf(x4);

		if (x5 > 1.0f)
		{
			x4 = -1.0;
			if (x4 > 0.0f)
				x4 = 1.0;
		}
	}

	return x4;
}

void Controller::feed(int stickNo, int touched, float x, float y)
{
	if (!isValidStick(stickNo))
		return;

	int index = (x >= 0.0f) ? 1 : 0;

	// maybe the 2 'touch sticks' are actually internally 1 single surface??

	isTouchedValues[index] = touched != 0;
	stickValuesX[index] = linearTransform(x + Controller_unk_1[index + 1], 0.0f, 2.78f, true);
	stickValuesY[index] = y;
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

bool Controller::isTouched(int stickNo)
{
	if (!isValidStick(stickNo))
		return false;

	return isTouchedValues[stickNo - 1];
}
