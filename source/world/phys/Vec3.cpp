/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Vec3.hpp"
#include "world/level/TilePos.hpp"

const Vec3 Vec3::ZERO = Vec3(0, 0, 0);

void Vec3::_init(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

Vec3::Vec3()
{
    _init(0, 0, 0);
}

Vec3::Vec3(float x, float y, float z)
{
    _init(x, y, z);
}

Vec3::Vec3(const TilePos& tilePos)
{
    _init(tilePos.x, tilePos.y, tilePos.z);
}

Vec3 Vec3::interpolateTo(const Vec3& to, float t) const
{
	float nx = x + (to.x - x) * t;
	float ny = y + (to.y - y) * t;
	float nz = z + (to.z - z) * t;
	return Vec3(nx, ny, nz);
}

Vec3 Vec3::vectorTo(const Vec3& to) const
{
	return Vec3(to.x - x, to.y - y, to.z - z);
}

Vec3 Vec3::normalize() const
{
	float dist = Mth::sqrt(x * x + y * y + z * z);
	if (dist < 0.0001f)
		return ZERO;

	return Vec3(x / dist, y / dist, z / dist);
}

float Vec3::dot(const Vec3& other) const
{
	return x * other.x + y * other.y + z * other.z;
}

Vec3 Vec3::cross(const Vec3& other) const
{
	return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}

Vec3 Vec3::add(float x, float y, float z) const
{
	return Vec3(this->x + x, this->y + y, this->z + z);
}

float Vec3::distanceTo(const Vec3& b) const
{
	return Mth::sqrt(distanceToSqr(b));
}

float Vec3::distanceToSqr(const Vec3& b) const
{
	Vec3 d = *this - b;

	return d.x * d.x + d.y * d.y + d.z * d.z;
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
