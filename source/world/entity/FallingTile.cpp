/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "FallingTile.hpp"
#include "world/level/Level.hpp"

FallingTile::FallingTile(Level* level) : Entity(level)
{
}

FallingTile::FallingTile(Level* level, float x, float y, float z, int id) : Entity(level)
{
	m_id = id;
	field_34 = 1;
	setSize(0.98f, 0.98f);
	field_84 = field_8C * 0.5f;
	setPos(x, y, z);
	field_3C.x = x;
	field_3C.y = y;
	field_3C.z = z;
	m_vel.x = 0.0f;
	m_vel.y = 0.0f;
	m_vel.z = 0.0f;

#if defined(ENH_ALLOW_SAND_GRAVITY)
	field_C8 = RENDER_FALLING_TILE;
#endif
}

float FallingTile::getShadowHeightOffs()
{
	return 0.0f;
}

bool FallingTile::isPickable()
{
	return !m_bRemoved;
}

void FallingTile::tick()
{
	if (!m_id)
		remove();

	field_3C = m_pos;
	field_E0++;

	m_vel.y -= 0.04f;
	move(m_vel.x, m_vel.y, m_vel.z);

	m_vel *= 0.98f;

	int tileX = Mth::floor(m_pos.x);
	int tileY = Mth::floor(m_pos.y);
	int tileZ = Mth::floor(m_pos.z);

	// if we're inside one of our own tiles, clear it.
	// Assumes we started there
	if (m_pLevel->getTile(tileX, tileY, tileZ) == m_id)
		m_pLevel->setTile(tileX, tileY, tileZ, TILE_AIR);

	if (!field_7C)
	{
		if (field_E0 > 100 && !m_pLevel->field_11)
			remove();

		return;
	}

	m_vel.x *= 0.7f;
	m_vel.z *= 0.7f;
	m_vel.y *= -0.5f;
	remove();
	if (m_pLevel->mayPlace(m_id, tileX, tileY, tileZ, true))
	{
		m_pLevel->setTile(tileX, tileY, tileZ, m_id);
	}
	else
	{
		// @TODO: spawn resources?
	}
}

Level* FallingTile::getLevel()
{
	return m_pLevel;
}
