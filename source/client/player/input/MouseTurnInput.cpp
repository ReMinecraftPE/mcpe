/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "MouseTurnInput.hpp"
#include "Mouse.hpp"

constexpr float C_SENSITIVITY = 1.0f;

MouseTurnInput::MouseTurnInput(Minecraft* pMC)
{
	m_pMinecraft = pMC;
	m_lastX = m_lastY = -1;
}

TurnDelta MouseTurnInput::getTurnDelta()
{
	int deltaX = 0, deltaY = 0;
	m_pMinecraft->platform()->getMouseDiff(deltaX, deltaY);
	m_pMinecraft->platform()->clearDiff();

	TurnDelta d;
	d.x = C_SENSITIVITY * deltaX;
	d.y = C_SENSITIVITY * deltaY;

	return d;
}
