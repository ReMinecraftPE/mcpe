/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "DoorTile.hpp"
#include "world/level/Level.hpp"
#include "world/item/Item.hpp"

DoorTile::DoorTile(int ID, Material* pMtl) : Tile(ID, pMtl)
{
	m_TextureFrame = TEXTURE_DOOR_BOTTOM;

	if (pMtl == Material::metal)
		m_TextureFrame = TEXTURE_DOOR_IRON_BOTTOM;

	Tile::setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
}

bool DoorTile::use(Level* level, const TilePos& pos, Player* player)
{
	// well, you know, iron doors can't be opened by right clicking
	if (m_pMaterial == Material::metal)
		return true;

	TileData data = level->getData(pos);

	// if we're the top tile
	if (data & 8)
	{
		if (level->getTile(pos.below()) == m_ID)
			use(level, pos.below(), player);
	}
	else
	{
		data ^= 4;
		if (level->getTile(pos.above()) == m_ID)
			level->setData(pos.above(), data + 8);

		level->setData(pos, data);

		// @BUG: marking the wrong tiles as dirty? No problem because setData sends an update immediately anyways
		level->setTilesDirty(pos.below(), pos);

		level->levelEvent(player, LevelEvent::SOUND_DOOR, pos);
	}

	return true;
}

void DoorTile::attack(Level* level, const TilePos& pos, Player* player)
{
	use(level, pos, player);
}

// @HUH: This function has NO references to itself. Not even in the vtable of the tile.
// This could have been an attempt by Mojang to override a function, but it clearly failed
// From this, I suspect that they never actually used the `override` keyword for their classes.
bool DoorTile::blocksLight() const
{
	return false;
}

HitResult DoorTile::clip(const Level* level, const TilePos& pos, Vec3 v1, Vec3 v2)
{
	// @NOTE: Tile::clip calls updateShape too. So this is redundant
	updateShape(level, pos);
	return Tile::clip(level, pos, v1, v2);
}

AABB* DoorTile::getAABB(const Level* level, const TilePos& pos)
{
	updateShape(level, pos);
	return Tile::getAABB(level, pos);
}

int DoorTile::getDir(TileData data) const
{
	if (!isOpen(data))
		return (data - 1) & 3;

	return data & 3;
}

int DoorTile::getRenderShape() const
{
	return SHAPE_DOOR;
}

int DoorTile::getResource(TileData data, Random* random) const
{
	// breaking the top of the tile doesn't drop anything.
	// In JE, it probably fixed a certain dupe glitch with doors
	if (isTop(data))
		return 0;

	if (m_pMaterial == Material::metal)
		return Item::door_wood->m_itemID;

	return Item::door_iron->m_itemID;
}

int DoorTile::getTexture(Facing::Name face, TileData data) const
{
	if (face == Facing::DOWN || face == Facing::UP)
		return m_TextureFrame;

	int doorDir = getDir(data);
	
	if ((doorDir == 0 || doorDir == 2) != (face <= Facing::SOUTH))
		return m_TextureFrame;

	int l = doorDir / 2 + ((face & 1) ^ doorDir);
	l += (data & 4) / 4;

	int idx = m_TextureFrame - (data & 8) * 2;
	if ((l & 1) != 0)
		idx = -idx;

	return idx;
}

AABB DoorTile::getTileAABB(const Level* level, const TilePos& pos)
{
	updateShape(level, pos);
	return Tile::getTileAABB(level, pos);
}

bool DoorTile::isCubeShaped() const
{
	return false;
}

bool DoorTile::isSolidRender() const
{
	return false;
}

bool DoorTile::mayPlace(const Level* level, const TilePos& pos) const
{
	return pos.y <= 126 && level->isSolidTile(pos.below()) && Tile::mayPlace(level, pos) && Tile::mayPlace(level, pos.above());
}

void DoorTile::setShape(int dir)
{
	// @NOTE: What? Why 2 tiles tall??
	Tile::setShape(0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 1.0f);
	switch (dir)
	{
		case 0:
			Tile::setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.1875f);
			break;
		case 1:
			Tile::setShape(0.8125f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
			break;
		case 2:
			Tile::setShape(0.0f, 0.0f, 0.8125f, 1.0f, 1.0f, 1.0f);
			break;
		case 3:
			Tile::setShape(0.0f, 0.0f, 0.0f, 0.1875f, 1.0f, 1.0f);
			break;
	}
}

void DoorTile::updateShape(const LevelSource* level, const TilePos& pos)
{
	setShape(getDir(level->getData(pos)));
}

void DoorTile::setOpen(Level* level, const TilePos& pos, bool bOpen)
{
	TileData data = level->getData(pos);
	if (isTop(data))
	{
		if (level->getTile(pos.below()) == m_ID)
			setOpen(level, pos.below(), bOpen);
		return;
	}

	if (isOpen(level->getData(pos)) != bOpen)
	{
		if (level->getTile(pos.above()) == m_ID)
			level->setData(pos.above(), (data ^ 4) + 8);

		level->setData(pos, data ^ 4);
		level->setTilesDirty(pos.below(), pos);

		std::string snd;
		if (Mth::random() < 0.5f)
			snd = "random.door_open";
		else
			snd = "random.door_close";

		level->playSound(Vec3(pos) + 0.5f, snd, 1.0f, 0.9f + 0.1f * level->m_random.nextFloat());
	}
}

void DoorTile::neighborChanged(Level* level, const TilePos& pos, TileID newTile)
{
	int isTop = level->getData(pos) & 8;
	if (isTop)
	{
		if (level->getTile(pos.below()) != m_ID)
			level->setTile(pos, TILE_AIR);

		if (newTile > 0)
		{
			if (Tile::tiles[newTile]->isSignalSource())
				neighborChanged(level, pos.below(), newTile);
		}

		return;
	}

	if (level->getTile(pos.above()) != m_ID)
	{
		level->setTile(pos, TILE_AIR);
		isTop = 1;
	}

	if (!level->isSolidTile(pos.below()))
	{
		level->setTile(pos, TILE_AIR);
		if (level->getTile(pos.above()) == m_ID)
			level->setTile(pos.above(), TILE_AIR);
	}

	if (!isTop && newTile > 0 && Tile::tiles[newTile]->isSignalSource())
	{
		bool bOpen = false;
		if (level->hasNeighborSignal(pos) || level->hasNeighborSignal(pos.above()))
			bOpen = true;

		setOpen(level, pos, bOpen);
	}
}
