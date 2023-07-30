/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

class ITurnInput
{
public:
	struct Delta
	{
		float x = 0.0f, y = 0.0f;
		Delta() {}
		Delta(float x, float y) : x(x), y(y) {}
	};

public:
	float getDeltaTime();
	virtual ~ITurnInput();
	virtual Delta getTurnDelta() = 0;

private:
	float m_prevTime = -1.0f;
};

