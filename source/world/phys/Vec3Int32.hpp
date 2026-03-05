/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "common/Mth.hpp"
#include "compat/LegacyCPP.hpp"
#include "world/phys/Vec3.hpp"

// TODO: Rename to `Pos`
struct Vec3Int32
{
public:
	int x, y, z;

public:
	Vec3Int32() : x(0), y(0), z(0) { }
	Vec3Int32(int xyz) : x(xyz), y(xyz), z(xyz) { }
	Vec3Int32(int x, int y, int z) : x(x), y(y), z(z) { }

	Vec3 toVec3() const
	{
		return Vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
	}

	float distanceTo(const Vec3Int32& b) const
	{
		return Mth::sqrt(float(distanceToSqr(b)));
	}

	int distanceToSqr(const Vec3Int32& b) const
	{
		return (*this - b).lengthSqr();
	}

	int lengthSqr() const
	{
		return x * x + y * y + z * z;
	}

	float length() const
	{
		return Mth::sqrt(float(lengthSqr()));
	}

	Vec3Int32 operator+(const Vec3Int32& b) const
	{
		return Vec3Int32(x + b.x, y + b.y, z + b.z);
	}

	Vec3Int32 operator-(const Vec3Int32& b) const
	{
		return Vec3Int32(x - b.x, y - b.y, z - b.z);
	}

	Vec3Int32 operator*(const Vec3Int32& b) const
	{
		return Vec3Int32(x * b.x, y * b.y, z * b.z);
	}

	void operator+=(const Vec3Int32& b)
	{
		x += b.x;
		y += b.y;
		z += b.z;
	}

	void operator-=(const Vec3Int32& b)
	{
		(*this) += -b;
	}

	void operator+=(int f)
	{
		x += f;
		y += f;
		z += f;
	}

	void operator-=(int f)
	{
		x -= f;
		y -= f;
		z -= f;
	}

	void operator*=(int f)
	{
		x *= f;
		y *= f;
		z *= f;
	}

	void operator/=(int f)
	{
		x /= f;
		y /= f;
		z /= f;
	}

	Vec3Int32 operator-() const
	{
		return Vec3Int32(-x, -y, -z);
	}

	Vec3Int32 operator-(int f) const
	{
		return Vec3Int32(x - f, y - f, z - f);
	}

	Vec3 operator-(float f) const
	{
		return Vec3(x - f, y - f, z - f);
	}

	Vec3Int32 operator*(int f) const
	{
		return Vec3Int32(x * f, y * f, z * f);
	}

	Vec3 operator*(float f) const
	{
		return Vec3(x * f, y * f, z * f);
	}

	Vec3Int32 operator/(int f) const
	{
		return Vec3Int32(x / f, y / f, z / f);
	}

	Vec3 operator/(float f) const
	{
		return Vec3(x / f, y / f, z / f);
	}

	bool operator==(const Vec3Int32& b) const
	{
		return x == b.x &&
			y == b.y &&
			z == b.z;
	}

	bool operator!=(const Vec3Int32& b) const
	{
		return x != b.x &&
			y != b.y &&
			z != b.z;
	}
};

