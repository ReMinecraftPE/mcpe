/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SmoothFloat.hpp"

SmoothFloat::SmoothFloat() :
	m_desiredValue(0),
	m_currentValue(0),
	m_velocity(0)
{
}

float SmoothFloat::getNewDeltaValue(float inputDelta, float speed)
{
	m_desiredValue += inputDelta;

	float v1 = (m_desiredValue - m_currentValue) * speed;
	m_velocity += 0.5f * (v1 - m_velocity);

	if ((v1 > 0.0f && m_velocity < v1) || (v1 < 0.0f && m_velocity > v1))
		v1 = m_velocity;

	m_currentValue += v1;

	return v1;
}
