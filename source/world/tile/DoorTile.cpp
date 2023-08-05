/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Tile.hpp"
#include "Item.hpp"
#include "Level.hpp"

DoorTile::DoorTile(int ID, Material* pMtl) : Tile(ID, pMtl)
{
	m_TextureFrame = TEXTURE_DOOR_BOTTOM;

	if (pMtl == Material::metal)
		m_TextureFrame = TEXTURE_DOOR_IRON_BOTTOM;

	Tile::setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
}

int DoorTile::use(Level* level, int x, int y, int z, Player* player)
{
	// well, you know, iron doors can't be opened by right clicking
	if (m_pMaterial == Material::metal)
		return 1;

	int data = level->getData(x, y, z);

	// if we're the top tile
	if (data & 8)
	{
		if (level->getTile(x, y - 1, z) == m_ID)
			use(level, x, y - 1, z, player);
	}
	else
	{
		data ^= 4;
		if (level->getTile(x, y + 1, z) == m_ID)
			level->setData(x, y + 1, z, data + 8);

		level->setData(x, y, z, data);

		// @BUG: marking the wrong tiles as dirty? No problem because setData sends an update immediately anyways
		level->setTilesDirty(x, y - 1, z, x, y, z);

		std::string snd;
		if (Mth::random() < 0.5f)
			snd = "random.door_open";
		else
			snd = "random.door_close";

		level->playSound(float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f, snd, 1.0f, 0.9f + 0.1f * level->field_38.nextFloat());
	}

	return 1;
}

void DoorTile::attack(Level* level, int x, int y, int z, Player* player)
{
	use(level, x, y, z, player);
}

// @HUH: This function has NO references to itself. Not even in the vtable of the tile.
// This could have been an attempt by Mojang to override a function, but it clearly failed
// From this, I suspect that they never actually used the `override` keyword for their classes.
bool DoorTile::blocksLight()
{
	return false;
}

HitResult DoorTile::clip(Level* level, int x, int y, int z, Vec3 v1, Vec3 v2)
{
	// @NOTE: Tile::clip calls updateShape too. So this is redundant
	updateShape(level, x, y, z);
	return Tile::clip(level, x, y, z, v1, v2);
}

AABB* DoorTile::getAABB(Level* level, int x, int y, int z)
{
	updateShape(level, x, y, z);
	return Tile::getAABB(level, x, y, z);
}

int DoorTile::getDir(int data)
{
	if (!isOpen(data))
		return (data - 1) & 3;

	return data & 3;
}

int DoorTile::getRenderShape()
{
	return SHAPE_DOOR;
}

int DoorTile::getResource(int data, Random* random)
{
	// breaking the top of the tile doesn't drop anything.
	// In JE, it probably fixed a certain dupe glitch with doors
	if (isTop(data))
		return 0;

	if (m_pMaterial == Material::metal)
		return Item::door_iron->m_itemID;

	return Item::door_wood->m_itemID;
}

int DoorTile::getTexture(int dir, int data)
{
	if (dir == 0 || dir == 1)
		return m_TextureFrame;

	int doorDir = getDir(data);
	
	if ((doorDir == 0 || doorDir == 2) != (dir <= 3))
		return m_TextureFrame;

	int l = doorDir / 2 + ((dir & 1) ^ doorDir);
	l += (data & 4) / 4;

	int idx = m_TextureFrame - (data & 8) * 2;
	if ((l & 1) != 0)
		idx = -idx;

	return idx;
}

AABB DoorTile::getTileAABB(Level* level, int x, int y, int z)
{
	updateShape(level, x, y, z);
	return Tile::getTileAABB(level, x, y, z);
}

bool DoorTile::isCubeShaped()
{
	return false;
}

bool DoorTile::isSolidRender()
{
	return false;
}

bool DoorTile::mayPlace(Level* level, int x, int y, int z)
{
	return y <= 126 && level->isSolidTile(x, y - 1, z) && Tile::mayPlace(level, x, y, z) && Tile::mayPlace(level, x, y + 1, z);
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

void DoorTile::updateShape(LevelSource* level, int x, int y, int z)
{
	setShape(getDir(level->getData(x, y, z)));
}

void DoorTile::setOpen(Level* level, int x, int y, int z, bool bOpen)
{
	int data = level->getData(x, y, z);
	if (isTop(data))
	{
		if (level->getTile(x, y - 1, z) == m_ID)
			setOpen(level, x, y - 1, z, bOpen);
		return;
	}

	if (isOpen(level->getData(x, y, z)) != bOpen)
	{
		if (level->getTile(x, y + 1, z) == m_ID)
			level->setData(x, y + 1, z, (data ^ 4) + 8);

		level->setData(x, y, z, data ^ 4);
		level->setTilesDirty(x, y - 1, z, x, y, z);

		std::string snd;
		if (Mth::random() < 0.5f)
			snd = "random.door_open";
		else
			snd = "random.door_close";

		level->playSound(float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f, snd, 1.0f, 0.9f + 0.1f * level->field_38.nextFloat());
	}
}

void DoorTile::neighborChanged(Level* level, int x, int y, int z, int newTile)
{
	int isTop = level->getData(x, y, z) & 8;
	if (isTop)
	{
		if (level->getTile(x, y - 1, z) != m_ID)
			level->setTile(x, y, z, TILE_AIR);

		if (newTile > 0)
		{
			if (Tile::tiles[newTile]->isSignalSource())
				neighborChanged(level, x, y - 1, z, newTile);
		}

		return;
	}

	if (level->getTile(x, y + 1, z) != m_ID)
	{
		level->setTile(x, y, z, TILE_AIR);
		isTop = 1;
	}

	if (!level->isSolidTile(x, y - 1, z))
	{
		level->setTile(x, y, z, TILE_AIR);
		if (level->getTile(x, y + 1, z) == m_ID)
			level->setTile(x, y + 1, z, TILE_AIR);
	}

	if (!isTop && newTile > 0 && Tile::tiles[newTile]->isSignalSource())
	{
		bool bOpen = false;
		if (level->hasNeighborSignal(x, y, z) || level->hasNeighborSignal(x, y + 1, z))
			bOpen = true;

		setOpen(level, x, y, z, bOpen);
	}
}
