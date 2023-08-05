/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Vec3.hpp"

class Entity;

class HitResult
{
public:
	// looks ass backwards, but what can you do about it
	enum eHitResultType
	{
		AABB,
		ENTITY,
		NONE,
	};

	enum eHitSide
	{
		NOHIT = -1,
		MINY  =  0,
		MAXY, // 1
		MINZ, // 2
		MAXZ, // 3
		MINX, // 4
		MAXX, // 5
	};

public:
	HitResult();
	HitResult(Entity*);
	HitResult(int x, int y, int z, eHitSide hitSide, const Vec3&);

public:
	eHitResultType m_hitType = NONE;
	// block coords?
	int m_tileX = 0;
	int m_tileY = 0;
	int m_tileZ = 0;

	eHitSide m_hitSide = MINY;

	// hit position
	Vec3 m_hitPos;
	
	Entity* m_pEnt = nullptr;
	bool m_bUnk24 = 0;
};

