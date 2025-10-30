/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "UnifiedTurnBuild.hpp"
#include "Multitouch.hpp"

#include "world/entity/Player.hpp"

UnifiedTurnBuild::UnifiedTurnBuild(int a, int width, int height, float d, float e, IInputHolder* pHolder) :
	field_C(a),
	field_10(0),
	m_screenArea(-1, -1, 0, 0),
	field_40(-1, -1, 0, 0),
	field_58(-1, -1, 0, 0),
	m_pInputHolder(pHolder),
	field_78(0.0f),
	field_7C(0.0f),
	m_bWasTouched(false),
	field_BC(0.0f),
	field_C0(d),
	field_C4(e),
	m_pPlayer(nullptr),
	field_D8(0)
{
	m_includeExcludeArea.field_4 = false;

	setScreenSize(width, height);
	field_B8 = getTimeS();
	field_CC = field_B8;
	field_D0 = 0;
	field_24 = false;
	field_D4 = false;
}

void UnifiedTurnBuild::setScreenSize(int width, int height)
{
	m_screenArea = RectangleArea(0.0f, 0.0f, float(width), float(height));

	field_40.right += 10.0f + 0.05f * (field_40.right - field_40.left);
	field_40.top   -= 10.0f + 0.05f * (field_40.bottom - field_40.top);

	m_includeExcludeArea.clear();
	m_includeExcludeArea.include(&m_screenArea);
	m_includeExcludeArea.exclude(&field_40);
	m_includeExcludeArea.exclude(&field_58);

	m_touchAreaModel.clear();
	m_touchAreaModel.addArea(100, &m_includeExcludeArea);
}

TurnDelta UnifiedTurnBuild::getTurnDelta()
{
	double timeS = getTimeS();

	float m1 = 0.0f, m2 = 0.0f;
	float xd = 0.0f, yd = 0.0f;
	bool touched = false;

	const int* activePtrs;
	int activePtrCount = Multitouch::getActivePointerIds(&activePtrs);

	for (int i = 0; i < activePtrCount; i++)
	{
		int finger = activePtrs[i];
		int x = Multitouch::getX(finger);
		int y = Multitouch::getY(finger);
		if (m_touchAreaModel.getPointerId(x, y, finger) == 100)
		{
			touched = true;
			m1 = float(x) * 0.5f;
			m2 = float(y) * -0.5f;
			break;
		}
	}

	if (touched && !m_bWasTouched) // if this is the first frame we were touched
	{
		field_B8 = timeS;
		field_BC = 0.0f;
		bool b2 = m_pPlayer && getSpeedSquared(m_pPlayer) > 0.01f;
		field_D8 = true;
		field_D4 = !b2;
		field_24 = false;
	}
	else if (m_bWasTouched && !touched) // if that was the last frame we were touched
	{
		field_24 = false;
		field_D8 = false;
	}

	if (field_C == 2 && (m_bWasTouched || touched))
	{
		// note: return value dropped
		(void) getDeltaTime();

		if (!m_bWasTouched)
		{
			field_78 = m1;
			field_7C = m2;
		}

		if (touched)
		{
			xd = field_C4 * linearTransform(m1 - field_78, 0.0f, 1.0f, false);
			yd = field_C4 * linearTransform(m2 - field_7C, 0.0f, 1.0f, false);

			float c3 = abs(xd) * abs(yd);
			if (field_C0 < c3)
				c3 = yd = xd = 0.0f;

			field_BC += c3;

			if (field_D8 == 1 && field_BC >= 20.0f)
				field_D8 = 2;

			if (field_D8 == 1 && timeS - field_B8 >= 0.4f)
			{
				bool check = m_pPlayer && getSpeedSquared(m_pPlayer) > 0.01f;

				if (field_BC > 20.0f || check)
					field_D8 = 2;
				else
					field_D8 = 3;
			}

			if (field_D8 == 1)
			{
				xd = yd = 0.0f;
			}

			field_78 = m1;
			field_7C = m2;
		}
	}

	if (field_D4)
	{
		// Yes, again, this is what IDA gave me. It was either a switch that the compiler
		// for some reason forgot to optimize into a jump table, or was actually an if chain.
		// I believe it's the latter though because the build I'm reversing (0.1.1j) is unoptimized)
		if (field_D8 == 1)
			m_pInputHolder->m_feedbackAlpha = m_smoothFloat.getNewDeltaValue((timeS - field_B8) / 0.4f, 0.05f);
		else if (field_D8 == 3)
			m_pInputHolder->m_feedbackAlpha = m_smoothFloat.getNewDeltaValue(1.0f, 0.25f);
		else if (field_D8 == 2)
			m_pInputHolder->m_feedbackAlpha = m_smoothFloat.getNewDeltaValue(-0.05f, 0.5f);
		else if (field_D8 == 0)
			m_pInputHolder->m_feedbackAlpha = m_smoothFloat.getNewDeltaValue(-0.05f, 0.5f);
	}
	else
	{
		m_pInputHolder->m_feedbackAlpha = -0.05f;
	}

	m_bWasTouched = touched;

	return TurnDelta(xd, -yd);
}

bool UnifiedTurnBuild::smoothTurning()
{
	return true;
}

bool UnifiedTurnBuild::tickBuild(Player* pPlayer, BuildActionIntention* pIntention)
{
	m_pPlayer = pPlayer;
	bool wroteIntention = false;
	BuildActionIntention::BuildActionIntent intent = BuildActionIntention::NONE;

	if (field_D8 == 3)
	{
		// 0.9.2
		/*if (field_106)
		{
			intent = BuildActionIntention::INTERACT;
			wroteIntention = true;
		}
		// Holds off on acknowledging interact intent until the user is absolutely sure a tick later
		else */if (field_24 /*&& pPlayer->isUsingItem()*/) // Adding pPlayer->isUsingItem() makes player break blocks way too fast when not holding items
		{
			intent = BuildActionIntention::TOUCH_HOLD_CONTINUE;
			wroteIntention = true;
		}
		else
		{
			intent = BuildActionIntention::TOUCH_HOLD_START;
			wroteIntention = true;
			// Next time, we acknowledge the action with intent to interact
			field_24 = true;
		}

		if (wroteIntention)
			*pIntention = BuildActionIntention(intent);

		return wroteIntention;
	}

	Multitouch::rewind();

	float timeS = getTimeS();
	field_10 = 0;

	while (Multitouch::next())
	{
		MouseAction* pEvent = Multitouch::getEvent();

		if (pEvent->_buttonType)
		{
			int finger = m_touchAreaModel.getPointerId(pEvent->_posX, pEvent->_posY, pEvent->_fingerId);

			if (finger == 100)
			{
				if (field_BC > 20.0f || pEvent->_buttonState || wroteIntention)
				{
					if (pEvent->_buttonState)
					{
						field_CC = timeS;
						field_D0 = 0;
						field_D8 = 1;
					}
				}
				else
				{
					float delta = timeS - field_CC;
					if (field_D8 <= 1 && delta >= 0.0f && delta < 0.25f)
					{
						intent = BuildActionIntention::TOUCH_TAP;
						wroteIntention = true;
					}

					field_D8 = 0;
				}
			}
		}
	}

	wroteIntention = true;
	if (wroteIntention)
		*pIntention = BuildActionIntention(intent);

	return wroteIntention;
}

bool UnifiedTurnBuild::isInsideArea(float x, float y)
{
	return m_includeExcludeArea.isInside(x, y);
}

float UnifiedTurnBuild::getSpeedSquared(Entity* pEnt)
{
	return (pEnt->m_pos - pEnt->m_oPos).lengthSqr();
}
