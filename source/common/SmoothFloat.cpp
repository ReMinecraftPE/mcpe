/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SmoothFloat.hpp"

SmoothFloat::SmoothFloat() :
	field_0(0),
	field_4(0),
	field_8(0)
{
}

float SmoothFloat::getNewDeltaValue(float a1, float a2)
{
	field_0 += a1;

	float v1 = (field_0 - field_4) * a2;
	field_8 += 0.5f * (v1 - field_8);

	if ((v1 > 0.0f && field_8 < v1) || (v1 < 0.0f && field_8 > v1))
		v1 = field_8;

	field_4 += v1;

	return v1;
}
