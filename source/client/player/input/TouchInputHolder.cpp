/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TouchInputHolder.hpp"
#include "Multitouch.hpp"
#include "client/app/Minecraft.hpp"
#include "client/options/Options.hpp"

TouchInputHolder::TouchInputHolder(Minecraft* pMinecraft, Options* pOptions) :
	m_touchScreenInput(pMinecraft, pOptions),
	m_unifiedTurnBuild(2, Minecraft::width, Minecraft::height, 200.0f, 1.05f, this),
	m_pMinecraft(pMinecraft)
{
}

bool TouchInputHolder::allowPicking()
{
	const int* ids;
	int count = Multitouch::getActivePointerIds(&ids);

	for (int i = 0; i < count; ++i)
	{
		int finger = ids[i];
		float x = float(Multitouch::getX(finger));
		float y = float(Multitouch::getY(finger));
		if (m_unifiedTurnBuild.isInsideArea(x, y))
		{
			m_feedbackX = x;
			m_feedbackY = y;
			return true;
		}
	}

	return false;
}

IMoveInput* TouchInputHolder::getMoveInput()
{
	return &m_touchScreenInput;
}

ITurnInput* TouchInputHolder::getTurnInput()
{
	return &m_unifiedTurnBuild;
}

IBuildInput* TouchInputHolder::getBuildInput()
{
	return &m_unifiedTurnBuild;
}

void TouchInputHolder::setScreenSize(int width, int height)
{
	m_touchScreenInput.setScreenSize(width, height);
	m_unifiedTurnBuild.field_40 = m_touchScreenInput.getRectangleArea();
	m_unifiedTurnBuild.field_58 = m_pMinecraft->m_pGui->getRectangleArea(false);
	m_unifiedTurnBuild.setScreenSize(width, height);
}
