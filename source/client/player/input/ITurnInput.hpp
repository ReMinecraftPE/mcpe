/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "compat/LegacyCPP.hpp"
#include "world/phys/Vec2.hpp"

class ITurnInput
{
public:
	ITurnInput()
	{
		m_prevTime = -1.0f;
	}
public:
	double getDeltaTime();
	float linearTransform(float, float, float, bool);

	virtual ~ITurnInput();
	virtual void setScreenSize(int width, int height);
	virtual Vec2 getTurnDelta() = 0;
	virtual bool smoothTurning();

private:
	double m_prevTime;
};

