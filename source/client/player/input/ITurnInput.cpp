/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ITurnInput.hpp"
#include "Utils.hpp"

float ITurnInput::getDeltaTime()
{
	if (m_prevTime == -1.0f)
		m_prevTime = getTimeS();

	float newTime = getTimeS();
	float delta = newTime - m_prevTime;
	m_prevTime = newTime;

	return delta;
}
