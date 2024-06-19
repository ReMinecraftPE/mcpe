/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "HitResult.hpp"

void HitResult::_init()
{
	m_hitType = NONE;
	m_tilePos = TilePos();

	m_hitSide = Facing::DOWN;

	m_pEnt = nullptr;
	m_bUnk24 = 0;
}

HitResult::HitResult(const TilePos& tilePos, Facing::Name hitSide, const Vec3& vec)
{
	_init();

	m_hitType = AABB;
	m_hitSide = hitSide;
	m_tilePos = tilePos;
	m_bUnk24 = false;
	m_hitPos = vec;
}

HitResult::HitResult(Entity* pEnt)
{
	_init();

	m_hitType = ENTITY;
	m_pEnt = pEnt;
}
