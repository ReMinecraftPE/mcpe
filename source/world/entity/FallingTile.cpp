/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "FallingTile.hpp"
#include "world/level/Level.hpp"
#include "nbt/CompoundTag.hpp"

FallingTile::FallingTile(Level* level) : Entity(level),
	m_id(TILE_AIR), // Uninitialized by Mojang
	field_E0(0)
{
}

FallingTile::FallingTile(Level* level, const Vec3& pos, int id) : Entity(level),
	field_E0(0)
{
	m_id = id;
    m_bBlocksBuilding = false;
	setSize(0.98f, 0.98f);
	m_heightOffset = m_bbHeight * 0.5f;
	setPos(pos);
	m_oPos = pos;
	m_bMakeStepSound = false;
	m_vel = Vec3::ZERO;

#if defined(ENH_ALLOW_SAND_GRAVITY)
	field_C8 = RENDER_FALLING_TILE;
#endif
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
	if (!m_id)
		remove();

	m_oPos = m_pos;
	field_E0++;

	m_vel.y -= 0.04f;
	move(m_vel);

	m_vel *= 0.98f;

	TilePos tilePos(m_pos);

	// if we're inside one of our own tiles, clear it.
	// Assumes we started there
	if (m_pLevel->getTile(tilePos) == m_id)
		m_pLevel->setTile(tilePos, TILE_AIR);

	if (!m_bOnGround)
	{
		if (field_E0 > 100 && !m_pLevel->m_bIsClientSide)
			remove();

		return;
	}

	m_vel.x *= 0.7f;
	m_vel.z *= 0.7f;
	m_vel.y *= -0.5f;
	remove();
	if (m_pLevel->mayPlace(m_id, tilePos, true))
	{
		m_pLevel->setTile(tilePos, m_id);
	}
	else
	{
		// @TODO: spawn resources?
	}
}

void FallingTile::addAdditionalSaveData(CompoundTag& tag) const
{
	tag.putInt8("Tile", m_id);
}

void FallingTile::readAdditionalSaveData(const CompoundTag& tag)
{
	m_id = tag.getInt8("Tile");
}