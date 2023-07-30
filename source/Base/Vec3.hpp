/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Mth.hpp"

class Vec3
{
public:
	float x = 0;
	float y = 0;
	float z = 0;

public:
	bool clipX(const Vec3& a2, float a3, Vec3& a4) const;
	bool clipY(const Vec3& a2, float a3, Vec3& a4) const;
	bool clipZ(const Vec3& a2, float a3, Vec3& a4) const;
	Vec3 normalize();

	// this constructor is nice to have, but it's probably inlined
	Vec3();
	Vec3(float x, float y, float z);

	// these are likely inlined
	float distanceTo(const Vec3& b) const;
	float distanceToSqr(const Vec3& b) const;

	// these are also likely inlined, but I'll declare them in the header
	Vec3 operator+(const Vec3& b)
	{
		return Vec3(x + b.x, y + b.y, z + b.z);
	}

	Vec3 operator-(const Vec3& b)
	{
		return Vec3(x - b.x, y - b.y, z - b.z);
	}

	void operator+=(const Vec3& b)
	{
		x += b.x;
		y += b.y;
		z += b.z;
	}

	void operator-=(const Vec3& b)
	{
		(*this) += -b;
	}

	void operator*=(float f)
	{
		x *= f;
		y *= f;
		z *= f;
	}

	Vec3 operator-() const
	{
		return Vec3(-x, -y, -z);
	}

	Vec3 operator*(float f) const
	{
		return Vec3(f * x, f * y, f * z);
	}

	Vec3 translate(float tx, float ty, float tz) const
	{
		return Vec3(x + tx, y + ty, z + tz);
	}
};

