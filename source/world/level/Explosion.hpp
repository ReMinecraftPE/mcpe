/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <set>
#include "Level.hpp"
#include "world/entity/Entity.hpp"

class Explosion
{
public:
	Explosion(Level*, Entity*, const Vec3& pos, float power);
	void addParticles();
	void explode();

	// @NOTE: This is inlined
	inline void setFiery(bool b)
	{
		m_bIsFiery = b;
	}

public:
	Vec3 m_pos;
	float m_power;

	//field_10, m_noEntityRenderFrames, m_totalEntities, m_renderedEntities - Likely a set<TilePos>
	std::set<TilePos> m_tiles;
	int field_20;

	bool m_bIsFiery;
	Entity* m_pEntity;
	Random m_random;
	Level* m_pLevel;

};

