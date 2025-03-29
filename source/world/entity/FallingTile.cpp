/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "FallingTile.hpp"
#include "world/level/Level.hpp"

void FallingTile::_init(TileID tileId, int tileData)
{
	m_pDescriptor = &EntityTypeDescriptor::fallingTile;

	m_tileId = tileId;
	m_tileData = tileData;
	m_time = 0;
	m_bBlocksBuilding = true;
	setSize(0.98f, 0.98f);
	m_heightOffset = m_bbHeight * 0.5f;
	m_bMakeStepSound = false;
	m_vel = Vec3::ZERO;

#if defined(ENH_ALLOW_SAND_GRAVITY)
	field_C8 = RENDER_FALLING_TILE;
#endif
}

void FallingTile::_init(const Vec3& pos, TileID tileId, int tileData)
{
	_init(tileId, tileData);

	setPos(pos);
	m_oPos = pos;
}

float FallingTile::getShadowHeightOffs() const
{
	return 0.0f;
}

bool FallingTile::isPickable() const
{
	return !m_bRemoved;
}

void FallingTile::tick()
{
	if (m_tileId == TILE_AIR)
	{
		remove();
		return;
	}

	m_oPos = m_pos;
	m_time++;

	m_vel.y -= 0.04f;
	move(m_vel);

	m_vel *= 0.98f;

	TilePos tilePos(m_pos);

	// if we're inside one of our own tiles, clear it.
	// Assumes we started there
	if (m_pLevel->getTile(tilePos) == m_tileId)
	{
		m_pLevel->removeTile(tilePos);
		LOG_I("%d: Removed fell tile", m_pLevel->getTime());
	}

	if (m_onGround)
	{
		m_vel.x *= 0.7f;
		m_vel.z *= 0.7f;
		m_vel.y *= -0.5f;
		remove();
		if ((!m_pLevel->mayPlace(m_tileId, tilePos, true) || !m_pLevel->setTileAndData(tilePos, m_tileId, m_tileData)) && !m_pLevel->m_bIsMultiplayer)
		{
			// Spawn resources
			spawnAtLocation(m_tileId, 1);
		}
	}
	else if (m_time > 100 && !m_pLevel->m_bIsMultiplayer)
	{
		// Spawn resources
		spawnAtLocation(m_tileId, 1);
		remove();
	}
}

Level* FallingTile::getLevel()
{
	return m_pLevel;
}
