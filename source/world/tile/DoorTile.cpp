/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "DoorTile.hpp"
#include "world/level/Level.hpp"
#include "world/level/TileSource.hpp"
#include "world/item/Item.hpp"

DoorTile::DoorTile(int ID, Material* pMtl) : Tile(ID, pMtl)
{
	m_TextureFrame = TEXTURE_DOOR_BOTTOM;
	m_renderLayer = RENDER_LAYER_ALPHATEST;

	if (pMtl == Material::metal)
		m_TextureFrame = TEXTURE_DOOR_IRON_BOTTOM;

	Tile::setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
}

bool DoorTile::use(const TilePos& pos, Player* player)
{
	// well, you know, iron doors can't be opened by right clicking
	if (m_pMaterial == Material::metal)
		return true;

	TileData data = source->getData(pos);

	// if we're the top tile
	if (data & 8)
	{
		if (source->getTile(pos.below()) == m_ID)
			use(source, pos.below(), player);
	}
	else
	{
		data ^= 4;
		if (source->getTile(pos.above()) == m_ID)
			source->setTileAndData(pos.above(), FullTile(m_ID, data + 8), TileChange::UPDATE_ALL | TileChange::UPDATE_UNK3);

		source->setTileAndData(pos, FullTile(m_ID, data), TileChange::UPDATE_ALL | TileChange::UPDATE_UNK3);

		// there is a fireTileChanged call here, but setTileAndData should already be calling that

		source->getLevel().levelEvent(LevelEvent(LevelEvent::SOUND_DOOR, pos, 0, player));
	}

	return true;
}

void DoorTile::attack(TileSource* source, const TilePos& pos, Player* player)
{
	use(source, pos, player);
}

// @HUH: This function has NO references to itself. Not even in the vtable of the tile.
// This could have been an attempt by Mojang to override a function, but it clearly failed
// From this, I suspect that they never actually used the `override` keyword for their classes.
bool DoorTile::blocksLight() const
{
	return false;
}

HitResult DoorTile::clip(TileSource* source, const TilePos& pos, Vec3 v1, Vec3 v2)
{
	// @NOTE: Tile::clip calls updateShape too. So this is redundant
	updateShape(source, pos);
	return Tile::clip(source, pos, v1, v2);
}

AABB* DoorTile::getAABB(TileSource* source, const TilePos& pos)
{
	updateShape(source, pos);
	return Tile::getAABB(source, pos);
}

int DoorTile::getDir(TileData data) const
{
	if (!isOpen(data))
		return (data - 1) & 3;

	return data & 3;
}

eRenderShape DoorTile::getRenderShape() const
{
	return SHAPE_DOOR;
}

int DoorTile::getResource(TileData data, Random* random) const
{
	// breaking the top of the tile doesn't drop anything in JE.
	// It probably fixed a certain dupe glitch with doors
	// if (isTop(data))
	// 	return 0;

	if (m_pMaterial == Material::metal)
		return Item::door_iron->m_itemID;

	return Item::door_wood->m_itemID;
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

AABB DoorTile::getTileAABB(TileSource* source, const TilePos& pos)
{
	updateShape(source, pos);
	return Tile::getTileAABB(source, pos);
}

bool DoorTile::isCubeShaped() const
{
	return false;
}

bool DoorTile::isSolidRender() const
{
	return false;
}

bool DoorTile::mayPlace(TileSource* source, const TilePos& pos) const
{
	return pos.y <= 126 && source->isSolidBlockingTile(pos.below()) && Tile::mayPlace(source, pos) && Tile::mayPlace(source, pos.above());
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

void DoorTile::updateShape(TileSource* source, const TilePos& pos)
{
	setShape(getDir(source->getData(pos)));
}

void DoorTile::setOpen(TileSource* source, const TilePos& pos, bool bOpen)
{
	TileData data = source->getData(pos);
	if (isTop(data))
	{
		if (source->getTile(pos.below()) == m_ID)
			setOpen(source, pos.below(), bOpen);
		return;
	}

	if (isOpen(source->getData(pos)) != bOpen)
	{
		if (source->getTile(pos.above()) == m_ID)
			source->setTileAndData(pos.above(), FullTile(m_ID, (data ^ 4) + 8), TileChange::UPDATE_ALL | TileChange::UPDATE_UNK3);

		source->setTileAndData(pos, FullTile(m_ID, data ^ 4), TileChange::UPDATE_ALL | TileChange::UPDATE_UNK3);

		// there is a fireTileChanged call here, but setTileAndData should already be calling that

		std::string snd;
		if (Mth::random() < 0.5f)
			snd = "random.door_open";
		else
			snd = "random.door_close";

		Level& level = source->getLevel();
		level.playSound(Vec3(pos) + 0.5f, snd, 1.0f, 0.9f + 0.1f * level.m_random.nextFloat());
	}
}

void DoorTile::neighborChanged(TileSource* source, const TilePos& pos, TileID newTile)
{
	int isTop = source->getData(pos) & 8;
	if (isTop)
	{
		if (source->getTile(pos.below()) != m_ID)
		{
			source->setTile(pos, TILE_AIR);
			spawnResources(source, pos, source->getData(pos));
		}

		if (newTile > 0)
		{
			if (Tile::tiles[newTile]->isSignalSource())
				neighborChanged(source, pos.below(), newTile);
		}

		return;
	}

	if (source->getTile(pos.above()) != m_ID)
	{
		source->setTile(pos, TILE_AIR);
		isTop = 1;
	}

	if (!source->isSolidBlockingTile(pos.below()))
	{
		source->setTile(pos, TILE_AIR);
		if (source->getTile(pos.above()) == m_ID)
		{
			source->setTile(pos.above(), TILE_AIR);
			spawnResources(source, pos, source->getData(pos));
		}
	}

	if (!isTop && newTile > 0 && Tile::tiles[newTile]->isSignalSource())
	{
		bool bOpen = false;
		if (source->hasNeighborSignal(pos) || source->hasNeighborSignal(pos.above()))
			bOpen = true;

		setOpen(source, pos, bOpen);
	}
}
