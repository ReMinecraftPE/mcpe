/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "compat/LegacyCPP.hpp"

struct TurnDelta
{
	float x, y;
	TurnDelta() { x = 0.0f; y = 0.0f; }
	TurnDelta(float x, float y) : x(x), y(y) {}
};

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
	virtual TurnDelta getTurnDelta() = 0;
	virtual bool smoothTurning();

private:
	double m_prevTime;
};

