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

private:
	void _init();
public:
	HitResult() { _init(); }
	HitResult(Entity*);
	HitResult(int x, int y, int z, eHitSide hitSide, const Vec3&);

public:
	eHitResultType m_hitType;
	// block coords?
	int m_tileX;
	int m_tileY;
	int m_tileZ;

	eHitSide m_hitSide;

	// hit position
	Vec3 m_hitPos;
	
	Entity* m_pEnt;
	bool m_bUnk24;
};

