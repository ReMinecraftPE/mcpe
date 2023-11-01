/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TouchInputHolder.hpp"
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
	m_unifiedTurnBuild.field_58 = m_pMinecraft->m_gui.getRectangleArea(false);
	m_unifiedTurnBuild.setScreenSize(width, height);
}
