/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Vec3.hpp"
#include "HitResult.hpp"

class AABB
{
public:
	Vec3 min, max;

	AABB();
	AABB(Vec3 min, Vec3 max);
	AABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);

public:
	HitResult clip(const Vec3&, const Vec3&);
	float clipXCollide(const AABB& bud, float f) const;
	float clipYCollide(const AABB& bud, float f) const;
	float clipZCollide(const AABB& bud, float f) const;

	bool containsX(Vec3* pVec);
	bool containsY(Vec3* pVec);
	bool containsZ(Vec3* pVec);

	bool intersect(const AABB& other) const;

	// @NOTE: Names for `move`, `grow` and `expand` were taken from really early minecraft (rd-132211 to be exact).
	void move(float x, float y, float z)
	{
		min += Vec3(x, y, z);
		max += Vec3(x, y, z);
	}

	// same thing
	void grow(float x, float y, float z)
	{
		min -= Vec3(x, y, z);
		max += Vec3(x, y, z);
	}

	// same thing
	void grow(float x)
	{
		min -= Vec3(x, x, x);
		max += Vec3(x, x, x);
	}

	void expand(float x, float y, float z)
	{
		if (x < 0) min.x += x;
		if (x > 0) max.x += x;
		if (y < 0) min.y += y;
		if (y > 0) max.y += y;
		if (z < 0) min.z += z;
		if (z > 0) max.z += z;
	}

	bool contains(const Vec3& v) const
	{
		if (v.x <= min.x || v.x >= max.x)
			return false;
		if (v.y > min.y && v.y < max.y)
			return v.z > min.z && v.z < max.z;
		return false;
	}
};

