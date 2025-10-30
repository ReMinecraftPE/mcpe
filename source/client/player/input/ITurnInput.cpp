/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ITurnInput.hpp"
#include "common/Utils.hpp"

ITurnInput::~ITurnInput()
{
}

void ITurnInput::setScreenSize(int width, int height)
{
}

bool ITurnInput::smoothTurning()
{
	return false;
}

double ITurnInput::getDeltaTime()
{
	if (m_prevTime == -1.0f)
		m_prevTime = getTimeS();

	double newTime = getTimeS();
	double delta = newTime - m_prevTime;
	m_prevTime = newTime;

	return delta;
}

// @TODO: Where does the a1 parameter come from? It's not `this` because it's used
// directly as a float. Its mangled name is _ZN10ITurnInput15linearTransformEfffb
float ITurnInput::linearTransform(float a1, float a2, float a3, bool a4)
{
	float v1;
	if (a1 < 0.0f)
		v1 = -a2;
	else
		v1 = a2;

	float v2 = abs(v1);
	if (v2 >= abs(a1))
		return 0.0f;

	float v3 = (a1 - v1) * a3;
	if (a4 && abs(v3) > 1.0f)
		v3 /= abs(v3);

	return v3;
}
