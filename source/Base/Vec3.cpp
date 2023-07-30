/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Vec3.hpp"

Vec3::Vec3() {}

Vec3::Vec3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vec3 Vec3::normalize()
{
	float dist = Mth::sqrt(x * x + y * y + z * z);
	if (dist < 0.0001f)
		return Vec3(0, 0, 0);

	return Vec3(x / dist, y / dist, z / dist);
}

bool Vec3::clipX(const Vec3& a2, float a3, Vec3& a4) const
{
	float crap = a2.x - this->x;
	if (crap * crap < 0.000001f)
		return false;

	float crap2 = (a3 - this->x) / crap;
	if (crap2 < 0.0f || crap2 > 1.0f)
		return false;

	a4.x = x + (a2.x - x) * crap2;
	a4.y = y + (a2.y - y) * crap2;
	a4.z = z + (a2.z - z) * crap2;
	return true;
}

bool Vec3::clipY(const Vec3& a2, float a3, Vec3& a4) const
{
	float crap = a2.y - this->y;
	if (crap * crap < 0.000001f)
		return false;

	float crap2 = (a3 - this->y) / crap;
	if (crap2 < 0.0f || crap2 > 1.0f)
		return false;

	a4.x = x + (a2.x - x) * crap2;
	a4.y = y + (a2.y - y) * crap2;
	a4.z = z + (a2.z - z) * crap2;
	return true;
}

bool Vec3::clipZ(const Vec3& a2, float a3, Vec3& a4) const
{
	float crap = a2.z - this->z;
	if (crap * crap < 0.000001f)
		return false;

	float crap2 = (a3 - this->z) / crap;
	if (crap2 < 0.0f || crap2 > 1.0f)
		return false;

	a4.x = x + (a2.x - x) * crap2;
	a4.y = y + (a2.y - y) * crap2;
	a4.z = z + (a2.z - z) * crap2;
	return true;
}

float Vec3::distanceToSqr(const Vec3& b) const
{
	float xd = (x-b.x);
	float yd = (y-b.y);
	float zd = (z-b.z);

	return xd * xd + yd * yd + zd * zd;
}

float Vec3::distanceTo(const Vec3& b) const
{
	return Mth::sqrt(distanceToSqr(b));
}
