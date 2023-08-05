/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ControllerTurnInput.hpp"
#include "Controller.hpp"

ITurnInput::Delta ControllerTurnInput::getTurnDelta()
{
	bool isTouched = Controller::isTouched(m_stickNo);
	float deltaX, deltaY;

	if (field_8 == 1)
	{
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
		deltaY = deltaTime * -yt;
	}
	else if (field_8 != 2 || !field_18 && !isTouched)
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
			float x1 = sx - field_10;
			float x2 = 0.0f;
			float x3;
			field_10 = sx;

			// @HUH: what the hell, decompiler?
			if (x1 >= 0.0f)
				x3 = 0.0f;
			else
				x3 = -0.0f;

			float x4 = sy - field_14;
			field_14 = sy;

			if (fabsf(x1) > 0.0f)
				x2 = x1 - x3;

			float ndy = 0.0f, x5;
			if (x4 >= 0.0f)
				x5 = 0.0f;
			else
				x5 = -0.0f;

			float x6 = 0.0f;
			if (fabsf(x4) > 0.0f)
				x6 = x4 - x5;

			deltaX = x2 * 100.0f;
			deltaY = -x6 * 100.0f;
		}
		else
		{
			deltaX = 0.0f;
			deltaY = -0.0f;
		}
	}

	field_18 = isTouched;
	return Delta(deltaX, deltaY);
}
