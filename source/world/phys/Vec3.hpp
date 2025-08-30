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

struct TilePos;

#define __VEC3_HPP

// NOTE: I don't think that Vec3 was implemented like that - it was
// probably implemented just like in Java. However, I think it looks
// nicer this way :)

class Vec3
{
public:
	static const Vec3 ZERO;

public:
	float x, y, z;
    
private:
    void _init(float x, float y, float z);

public:
	// this constructor is nice to have, but it's probably inlined
	Vec3();
	Vec3(float x, float y, float z);
	Vec3(const TilePos& tilePos);

	Vec3 interpolateTo(const Vec3& to, float t) const;
	Vec3 vectorTo(const Vec3& to) const
	{
		return Vec3(to.x - x, to.y - y, to.z - z);
	}
	Vec3 normalize() const;
	float dot(const Vec3& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}
	Vec3 cross(const Vec3& other) const
	{
		return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
	}
	Vec3 add(float x, float y, float z) const
	{
		return Vec3(this->x + x, this->y + y, this->z + z); 
	}

	// these are likely inlined
	float distanceTo(const Vec3& b) const
	{ 
		return Mth::sqrt(distanceToSqr(b));
	}
	float distanceToSqr(const Vec3& b) const
	{
		return (*this - b).lengthSqr();
	}
    
    bool clipX(const Vec3& a2, float a3, Vec3& a4) const;
	bool clipY(const Vec3& a2, float a3, Vec3& a4) const;
	bool clipZ(const Vec3& a2, float a3, Vec3& a4) const;

	Vec3 translate(float tx, float ty, float tz) const
	{
		return Vec3(x + tx, y + ty, z + tz);
	}

	float lengthSqr() const
	{
		return x * x + y * y + z * z;
	}

	float length() const
	{
		return Mth::sqrt(lengthSqr());
	}

	Vec3 scale(float scale) const
	{
		return Vec3(x * scale, y * scale, z * scale);
	}

	// these are also likely inlined, but I'll declare them in the header
	Vec3 operator+(const Vec3& b) const
	{
		return Vec3(x + b.x, y + b.y, z + b.z);
	}

	Vec3 operator+(float f) const
	{
		return *this + Vec3(f, f, f);
	}

	Vec3 operator-(const Vec3& b) const
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

	void operator+=(float f)
	{
		x += f;
		y += f;
		z += f;
	}

	void operator-=(float f)
	{
		x -= f;
		y -= f;
		z -= f;
	}

	void operator*=(float f)
	{
		x *= f;
		y *= f;
		z *= f;
	}

	void operator/=(float f)
	{
		x /= f;
		y /= f;
		z /= f;
	}

	Vec3 operator-() const
	{
		return Vec3(-x, -y, -z);
	}

	Vec3 operator-(float f) const
	{
		return Vec3(x - f, y - f, z - f);
	}

	Vec3 operator*(float f) const
	{
		return Vec3(x * f, y * f, z * f);
	}

	Vec3 operator/(float f) const
	{
		return Vec3(x / f, y / f, z / f);
	}

	bool operator==(const Vec3& b) const
	{
		return x == b.x &&
			   y == b.y &&
			   z == b.z;
	}
};

