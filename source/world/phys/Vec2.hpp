/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "common/Mth.hpp"
// Needed for when we're missing nullptr in multiple files
#include "compat/LegacyCPP.hpp"

class Vec2
{
public:
	static const Vec2 ZERO;

public:
	float x, y;
    
private:
    void _init(float x, float y);

public:
	// this constructor is nice to have, but it's probably inlined
	Vec2();
	Vec2(float x, float y);
    
    Vec2 normalize() const;

	// these are also likely inlined, but I'll declare them in the header
	Vec2 operator+(const Vec2& b) const
	{
		return Vec2(x + b.x, y + b.y);
	}

	Vec2 operator-(const Vec2& b) const
	{
		return Vec2(x - b.x, y - b.y);
	}

	void operator+=(const Vec2& b)
	{
		x += b.x;
		y += b.y;
	}

	void operator-=(const Vec2& b)
	{
		(*this) += -b;
	}

	void operator*=(float f)
	{
		x *= f;
		y *= f;
	}

	Vec2 operator-() const
	{
		return Vec2(-x, -y);
	}

	Vec2 operator*(float f) const
	{
		return Vec2(f * x, f * y);
	}

	Vec2 operator/(float f) const
	{
		return Vec2(x / f, y / f);
	}

	bool operator==(const Vec2& b) const
	{
		return x == b.x &&
			   y == b.y;
	}

	Vec2 translate(float tx, float ty) const
	{
		return Vec2(x + tx, y + ty);
	}

	float lengthSqr() const
	{
		return x * x + y * y;
	}

	float length() const
	{
		return Mth::sqrt(lengthSqr());
	}

	Vec2 scale(float scale) const
	{
		return Vec2(x * scale, y * scale);
	}
};

