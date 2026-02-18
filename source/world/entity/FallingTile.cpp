/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "FallingTile.hpp"
#include "world/level/Level.hpp"
#include "nbt/CompoundTag.hpp"

#define DATA_TILE_ID (20)

void FallingTile::_init(Level*, const Vec3& pos, int id)
{
	m_renderType = RENDER_FALLING_TILE;
	m_pDescriptor = &EntityTypeDescriptor::fallingTile;

	_defineEntityData();

	setTile(id);
	field_E0 = 0;
	m_bBlocksBuilding = false;
	setSize(0.98f, 0.98f);
	m_heightOffset = m_bbHeight * 0.5f;
	setPos(pos);
	m_oPos = pos;
	m_bMakeStepSound = false;
	m_vel = Vec3::ZERO;
}

FallingTile::FallingTile(Level* level) : Entity(level)
{
	_init(level, Vec3::ZERO, TILE_AIR);
}

FallingTile::FallingTile(Level* level, const Vec3& pos, int id) : Entity(level)
{
	_init(level, pos, id);
}

void FallingTile::_defineEntityData()
{
	m_entityData.define<int32_t>(DATA_TILE_ID, TILE_AIR);
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
	if (getTile() == TILE_AIR)
		remove();

	m_oPos = m_pos;
	field_E0++;

	m_vel.y -= 0.04f;
	move(m_vel);

	m_vel *= 0.98f;

	TilePos tilePos(m_pos);

	// if we're inside one of our own tiles, clear it.
	// Assumes we started there
	if (m_pLevel->getTile(tilePos) == getTile())
		m_pLevel->setTile(tilePos, TILE_AIR);

	if (!m_bOnGround)
	{
		if (field_E0 > 100 && !m_pLevel->m_bIsClientSide)
		{
			spawnAtLocation(m_id, 1);
			remove();
		}

		return;
	}

	m_vel.x *= 0.7f;
	m_vel.z *= 0.7f;
	m_vel.y *= -0.5f;
	remove();
	if (m_pLevel->mayPlace(getTile(), tilePos, true))
	{
		m_pLevel->setTile(tilePos, getTile());
	}
	else
	{
		spawnAtLocation(getTile());
	}
}

void FallingTile::addAdditionalSaveData(CompoundTag& tag) const
{
	tag.putInt8("Tile", getTile());
}

void FallingTile::readAdditionalSaveData(const CompoundTag& tag)
{
	setTile(tag.getInt8("Tile"));
}

int FallingTile::getTile() const
{
	return m_entityData.get<int32_t>(DATA_TILE_ID);
}

void FallingTile::setTile(int id)
{
	m_entityData.set<int32_t>(DATA_TILE_ID, id);
}