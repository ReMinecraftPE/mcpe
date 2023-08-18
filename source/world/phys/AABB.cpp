/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "AABB.hpp"

AABB::AABB()
{
}

AABB::AABB(Vec3 _min, Vec3 _max) :
	min(_min), max(_max)
{
}

AABB::AABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) :
	min(minX, minY, minZ), max(maxX, maxY, maxZ)
{
}

HitResult AABB::clip(const Vec3& vec1, const Vec3& vec2)
{
	Vec3 clipMinX, clipMinY, clipMinZ;
	Vec3 clipMaxX, clipMaxY, clipMaxZ;
	bool bClipMinX, bClipMinY, bClipMinZ;
	bool bClipMaxX, bClipMaxY, bClipMaxZ;

	bClipMinX = vec1.clipX(vec2, min.x, clipMinX) && containsX(&clipMinX);
	bClipMaxX = vec1.clipX(vec2, max.x, clipMaxX) && containsX(&clipMaxX);
	bClipMinY = vec1.clipY(vec2, min.y, clipMinY) && containsY(&clipMinY);
	bClipMaxY = vec1.clipY(vec2, max.y, clipMaxY) && containsY(&clipMaxY);
	bClipMinZ = vec1.clipZ(vec2, min.z, clipMinZ) && containsZ(&clipMinZ);
	bClipMaxZ = vec1.clipZ(vec2, max.z, clipMaxZ) && containsZ(&clipMaxZ);

	// the collided side of our AABB
	HitResult::eHitSide collType = HitResult::NOHIT;

	// the preferred vector for our collision
	Vec3* pVec = nullptr;
	if (bClipMinX)
		pVec = &clipMinX, collType = HitResult::MINX;

	if (bClipMaxX)
	{
		if (!pVec || clipMaxX.distanceToSqr(vec1) < pVec->distanceToSqr(vec1))
			pVec = &clipMaxX, collType = HitResult::MAXX;
	}

	if (bClipMinY)
	{
		if (!pVec || clipMinY.distanceToSqr(vec1) < pVec->distanceToSqr(vec1))
			pVec = &clipMinY, collType = HitResult::MINY;
	}

	if (bClipMaxY)
	{
		if (!pVec || clipMaxY.distanceToSqr(vec1) < pVec->distanceToSqr(vec1))
			pVec = &clipMaxY, collType = HitResult::MAXY;
	}

	if (bClipMinZ)
	{
		if (!pVec || clipMinZ.distanceToSqr(vec1) < pVec->distanceToSqr(vec1))
			pVec = &clipMinZ, collType = HitResult::MINZ;
	}

	if (bClipMaxZ)
	{
		if (!pVec || clipMaxZ.distanceToSqr(vec1) < pVec->distanceToSqr(vec1))
			pVec = &clipMaxZ, collType = HitResult::MAXZ;
	}

	if (!pVec)
	{
		// return a nothing burger
		return HitResult();
	}

	return HitResult(0, 0, 0, collType, *pVec);
}

float AABB::clipXCollide(const AABB& bud, float f) const
{
	if (bud.max.y > min.y && bud.min.y < max.y && bud.max.z > min.z && bud.min.z < max.z)
	{
		if (f > 0.0f)
		{
			if (bud.max.x <= min.x)
				f = Mth::Min(min.x - bud.max.x, f);
		}

		if (f < 0.0f)
		{
			if (bud.min.x >= max.x)
				f = Mth::Max(max.x - bud.min.x, f);
		}
	}

	return f;
}

float AABB::clipYCollide(const AABB& bud, float f) const
{
	if (bud.max.x > min.x && bud.min.x < max.x && bud.max.z > min.z && bud.min.z < max.z)
	{
		if (f > 0.0f)
		{
			if (bud.max.y <= min.y)
				f = Mth::Min(min.y - bud.max.y, f);
		}

		if (f < 0.0f)
		{
			if (bud.min.y >= max.y)
				f = Mth::Max(max.y - bud.min.y, f);
		}
	}

	return f;
}

float AABB::clipZCollide(const AABB& bud, float f) const
{
	if (bud.max.x > min.x && bud.min.x < max.x && bud.max.y > min.y && bud.min.y < max.y)
	{
		if (f > 0.0f)
		{
			if (bud.max.z <= min.z)
				f = Mth::Min(min.z - bud.max.z, f);
		}

		if (f < 0.0f)
		{
			if (bud.min.z >= max.z)
				f = Mth::Max(max.z - bud.min.z, f);
		}
	}

	return f;
}

bool AABB::containsX(Vec3* pVec)
{
	return pVec->y >= this->min.y && pVec->y <= this->max.y && pVec->z >= this->min.z && pVec->z <= this->max.z;
}

bool AABB::containsY(Vec3* pVec)
{
	return pVec->x >= this->min.x && pVec->x <= this->max.x && pVec->z >= this->min.z && pVec->z <= this->max.z;
}

bool AABB::containsZ(Vec3* pVec)
{
	return pVec->x >= this->min.x && pVec->x <= this->max.x && pVec->y >= this->min.y && pVec->y <= this->max.y;
}

bool AABB::intersect(const AABB& other) const
{
	return max.x > other.min.x
		&& min.x < other.max.x
		&& max.y > other.min.y
		&& min.y < other.max.y
		&& max.z > other.min.z
		&& min.z < other.max.z;
}
