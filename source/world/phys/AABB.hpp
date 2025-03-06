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
	void move(const Vec3& vec);
	void move(float x, float y, float z);
	// same thing
	void grow(const Vec3& vec);
	void grow(float x, float y, float z);
	// same thing
	void grow(float x);
	void expand(float x, float y, float z);
	void expand(const Vec3& vec);
	bool contains(const Vec3& v) const;
};

