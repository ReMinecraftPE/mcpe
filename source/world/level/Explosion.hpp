/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <set>
#include "Level.hpp"
#include "Entity.hpp"

class Explosion
{
public:
	Explosion(Level*, Entity*, float x, float y, float z, float power);
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

	//field_10, field_14, field_18, field_1C - Likely a set<TilePos>
	std::set<TilePos> m_tiles;
	int field_20 = 0;

	bool m_bIsFiery = false;
	Entity* m_pEntity = nullptr;
	Random m_random;
	Level* m_pLevel;

};

