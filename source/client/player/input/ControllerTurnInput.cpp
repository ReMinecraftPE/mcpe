/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <cmath>

#include "ControllerTurnInput.hpp"
#include "Controller.hpp"

ControllerTurnInput::ControllerTurnInput()
{
	field_8 = 1; // 2 by default in 0.9.2, but this just causes the camera to lock to the joystick's axis at all times
	m_stickNo = 2;
	field_10 = 0.0f;
	field_14 = 0.0f;
	field_18 = false;
}

TurnDelta ControllerTurnInput::getTurnDelta()
{
#ifdef USE_NATIVE_ANDROID
	return TurnDelta(Controller::getX(m_stickNo) * 50.f, Controller::getY(m_stickNo) * 60.f);
#endif

	bool isTouched = Controller::isTouched(m_stickNo);
	float deltaX, deltaY;

	if (field_8 == 1)
	{
		if (Controller::isReset()) getDeltaTime();

		float deltaTime = getDeltaTime(), dx, dy;
		if (isTouched)
		{
			dx = Controller::getX(m_stickNo);
			dy = Controller::getY(m_stickNo);
		}
		else
		{
			dx = field_10 * 0.7f;
			dy = field_14 * 0.7f;
		}

		field_10 = dx;
		field_14 = dy;

		float xs = dx >= 0.0f ? 0.1f : -0.1f, xt;

		if (fabsf(dx) <= 0.1f)
		{
			xt = 0.0f;
		}
		else
		{
			xs = dx - xs;
			xt = 250.0f;
			xt = xs * xt;
		}

		float ys = dy >= 0.0f ? 0.1f : -0.1f, yt;

		if (fabsf(dy) <= 0.1f)
		{
			yt = 0.0f;
		}
		else
		{
			ys = dy - ys;
			yt = 200.0f;
			yt = ys * yt;
		}

		deltaX = deltaTime * xt;
		deltaY = deltaTime * yt; // inverted on 0.9.2
	}
    else if (field_8 != 2 || (!field_18 && !isTouched))
	{
		deltaX = 0.0f;
		deltaY = -0.0f;
	}
	else
	{
		float sx = Controller::getX(m_stickNo);
		float sy = Controller::getY(m_stickNo);

		getDeltaTime(); //@NOTE: call isn't useless since it updates m_prevTime

		if (!field_18)
		{
			field_10 = sx;
			field_14 = sy;
		}

		if (isTouched)
		{
			float diffX = sx - field_10;
			float diffY = sy - field_14;

			deltaX = fabsf(diffX) > 0.0f ? diffX * 100.0f : 0.0f;
			deltaY = fabsf(diffY) > 0.0f ? diffY * 100.0f : 0.0f; // deltaY is inverted on 0.9.2
			field_10 = sx;
			field_14 = sy;
		}
		else
		{
			deltaX = 0.0f;
			deltaY = -0.0f;
		}
	}

	field_18 = isTouched;
	return TurnDelta(deltaX, deltaY);
}

bool ControllerTurnInput::smoothTurning()
{
	return true;
}
