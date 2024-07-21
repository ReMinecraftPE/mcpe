/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <cmath>

#include "ControllerTurnInput.hpp"
#include "Controller.hpp"
#include "common/Logger.hpp"

ControllerTurnInput::ControllerTurnInput()
{
	field_8 = 1; // 2 by default in 0.9.2, but this just causes the camera to lock to the joystick's axis at all times
	m_stickNo = 2;
	m_analogTurnVector = Vec2::ZERO;
	field_18 = false;
}

/**
 * @BUG: Despite receiving consistent X and Y from the controller, the returned value will consistently fluctuate.
 * This results in inconsistently-jittery turn movement, as well as well as a non-smooth turning experience overall.
 * Simply by taking the square root of the controller X and Y and multiplying the result of each, we get an infinitely-
 * smoother and more stable end-user experience. The only problem: this is tied to FPS rather than proper timing.
**/
TurnDelta ControllerTurnInput::getTurnDelta()
{
#ifdef USE_NATIVE_ANDROID
	return TurnDelta(Controller::getX(m_stickNo) * 50.f, Controller::getY(m_stickNo) * 60.f);
#endif

#if 0
	constexpr float targetFps = 60;
	constexpr float mult = (60.0f / targetFps) * 16.0f;

	TurnDelta delta(Controller::getX(m_stickNo), Controller::getY(m_stickNo));
	delta.x = (delta.x * fabs(delta.x)) * mult;
	delta.y = (delta.y * fabs(delta.y)) * mult;
	return delta;
#endif

	bool isTouched = Controller::isTouched(m_stickNo);
	float deltaX, deltaY;

	if (field_8 == 1)
	{
		if (Controller::isReset())
			getDeltaTime();

		double deltaTime = getDeltaTime();
	    LOG_I("deltaTime: %f", deltaTime);
		if (isTouched)
		{
			m_analogTurnVector.x = Controller::getX(m_stickNo);
			m_analogTurnVector.y = Controller::getY(m_stickNo);
		}
		else
		{
			m_analogTurnVector.x *= 0.7f;
			m_analogTurnVector.y *= 0.7f;
		}
		
		// Deadzone handling moved to Controller
		//double f = pow(5.0 * 0.6f + 0.2f, 3) * 14.0f * 1.0f; // Legacy 4J
		//float xt = m_analogTurnVector.x * fabs(m_analogTurnVector.x) * 289.0f; // 250.0f for PE, 17.0f for LCE?
		//float yt = m_analogTurnVector.y * fabs(m_analogTurnVector.y) * 289.0f; // 200.0f for PE, 17.0f for LCE?

		float turnSpeed = 50.0f * 5.8f;
		float tx = m_analogTurnVector.x * Mth::abs(m_analogTurnVector.x) * turnSpeed;
		float ty = m_analogTurnVector.y * Mth::abs(m_analogTurnVector.y) * turnSpeed;

		/*xt *= fabs(xt);
		yt *= fabs(yt);*/

		deltaX = deltaTime * tx;
		deltaY = deltaTime * ty; // inverted on 0.9.2
		/*deltaX *= fabs(deltaX);
		deltaY *= fabs(deltaY);*/
	}
	else if (field_8 == 2 && (field_18 || isTouched))
	{
		float sx = Controller::getX(m_stickNo);
		float sy = Controller::getY(m_stickNo);

		getDeltaTime(); //@NOTE: call isn't useless since it updates m_prevTime

		if (!field_18)
		{
			m_analogTurnVector.x = sx;
			m_analogTurnVector.y = sy;
		}

		if (isTouched)
		{
			float diffX = sx - m_analogTurnVector.x;
			float diffY = sy - m_analogTurnVector.y;

			deltaX = fabsf(diffX) > 0.0f ? diffX * 100.0f : 0.0f;
			deltaY = fabsf(diffY) > 0.0f ? diffY * 100.0f : 0.0f; // deltaY is inverted on 0.9.2
			m_analogTurnVector.x = sx;
			m_analogTurnVector.y = sy;
		}
		else
		{
			deltaX = 0.0f;
			deltaY = -0.0f;
		}
	}
	else
	{
		deltaX = 0.0f;
		deltaY = -0.0f;
	}

	field_18 = isTouched;
	return TurnDelta(deltaX, deltaY);
}

bool ControllerTurnInput::smoothTurning()
{
	return true;
}
