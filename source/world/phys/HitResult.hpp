/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Vec3.hpp"
#include "world/level/TilePos.hpp"
#include "world/Facing.hpp"

class Entity;

class HitResult
{
public:
	// looks ass backwards, but what can you do about it
	enum HitResultType
	{
		AABB,
		ENTITY,
		NONE,
	};

	// Replaced with Facing::Name
	/*enum HitSide //: signed char
	{
		NOHIT = -1,
		MINY  =  0,
		MAXY, // 1
		MINZ, // 2
		MAXZ, // 3
		MINX, // 4
		MAXX, // 5
	};*/

private:
	void _init();
public:
	HitResult() { _init(); }
	HitResult(Entity*);
	HitResult(const TilePos& tilePos, Facing::Name hitSide, const Vec3&);
	bool isHit() const {
		return m_hitType != NONE;
	}

public:
	HitResultType m_hitType;
	TilePos m_tilePos;

	Facing::Name m_hitSide;

	// hit position
	Vec3 m_hitPos;
	
	Entity* m_pEnt;
	bool m_bUnk24;
};

