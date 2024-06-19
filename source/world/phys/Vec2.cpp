/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Vec2.hpp"

const Vec2 Vec2::ZERO = Vec2(0, 0);

Vec2::Vec2()
	: Vec2(ZERO) {}

Vec2::Vec2(float x, float y)
	: x(x), y(y) {}

Vec2 Vec2::normalize()
{
	float dist = Mth::sqrt(x * x + y * y);
	if (dist < 0.0001f)
		return ZERO;

	return Vec2(x / dist, y / dist);
}
