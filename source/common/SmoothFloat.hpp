/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

class SmoothFloat
{
public:
	SmoothFloat();

public:
	float getNewDeltaValue(float inputDelta, float speed);

private:
	float m_desiredValue;
	float m_currentValue;
	float m_velocity;
};

