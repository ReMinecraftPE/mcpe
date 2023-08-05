/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "HitResult.hpp"

HitResult::HitResult() {}

HitResult::HitResult(int x, int y, int z, eHitSide hitSide, const Vec3& vec)
{
	m_hitType = AABB;
	m_hitSide = hitSide;
	m_tileX = x;
	m_tileY = y;
	m_tileZ = z;
	m_bUnk24 = false;
	m_hitPos = vec;
}

HitResult::HitResult(Entity* pEnt)
{
	m_hitType = ENTITY;
	m_pEnt = pEnt;
}
