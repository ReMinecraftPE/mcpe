/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <climits>

#include "Vec2.hpp"

const Vec2 Vec2::ZERO = Vec2(0, 0), Vec2::ONE = Vec2(1, 1);
const Vec2 Vec2::UNIT_X = Vec2(1, 0), Vec2::NEG_UNIT_X = Vec2(-1, 0);
const Vec2 Vec2::UNIT_Y = Vec2(0, 1), Vec2::NEG_UNIT_Y = Vec2(0, -1);
const Vec2 Vec2::MIN = Vec2(INT_MIN, INT_MIN), Vec2::MAX = Vec2(INT_MAX, INT_MAX);

void Vec2::_init(float x, float y)
{
    this->x = x;
    this->y = y;
}

Vec2::Vec2()
{
    _init(0, 0);
}

Vec2::Vec2(float xy)
{
    _init(xy, xy);
}

Vec2::Vec2(float x, float y)
{
    _init(x, y);
}

Vec2 Vec2::normalize() const
{
	float dist = Mth::sqrt(x * x + y * y);
	if (dist < 0.0001f)
		return ZERO;

	return Vec2(x / dist, y / dist);
}
