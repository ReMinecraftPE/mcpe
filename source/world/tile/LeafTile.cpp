/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LeafTile.hpp"
#include "world/level/Level.hpp"
#include "client/renderer/PatchManager.hpp"

LeafTile::LeafTile(int id) : TransparentTile(id, TEXTURE_LEAVES_TRANSPARENT, Material::leaves, false)
{
	field_70 = nullptr;

	m_TextureFrame = TEXTURE_LEAVES_TRANSPARENT;
	field_74 = TEXTURE_LEAVES_TRANSPARENT;

	setTicking(true);
}

LeafTile::~LeafTile()
{
	if (field_70)
		delete[] field_70;
}

void LeafTile::die(Level* level, const TilePos& pos)
{
	spawnResources(level, pos, level->getData(pos));
	level->setTile(pos, TILE_AIR);
}

int LeafTile::getColor(const LevelSource* level, const TilePos& pos) const
{
	if (GetPatchManager()->IsGrassTinted())
	{
		return 0x339933;
	}

	return 0xffffff;
}

int LeafTile::getTexture(Facing::Name face, int data) const
{
	if ((data & 3) == 1)
		return m_TextureFrame + 80;

	return m_TextureFrame;
}

bool LeafTile::isSolidRender() const
{
	return !m_bTransparent;
}

void LeafTile::stepOn(Level* level, const TilePos& pos, Entity* entity)
{
}

void LeafTile::onRemove(Level* level, const TilePos& pos)
{
	if (!level->hasChunksAt(pos - 2, pos + 2))
		return;

	TilePos o(-1, -1, -1);
	for (o.x = -1; o.x < 2; o.x++)
	{
		for (o.y = -1; o.y < 2; o.y++)
		{
			for (o.z = -1; o.z < 2; o.z++)
			{
				TileID tile = level->getTile(pos + o);
				if (tile != Tile::leaves->m_ID) continue;

				level->setDataNoUpdate(pos + o, level->getData(pos + o) | 4);
			}
		}
	}
}

void LeafTile::tick(Level* level, const TilePos& pos, Random* random)
{
	if (level->m_bIsMultiplayer)
		return;

	int data = level->getData(pos);
	if ((data & 4) == 0)
		return;

	constexpr int C_RANGE = 32;
	constexpr int C_RANGE_SMALL = 4;

	if (!field_70)
		field_70 = new int[C_RANGE * C_RANGE * C_RANGE];

	if (level->hasChunksAt(pos - 5, pos + 5))
	{
		TilePos curr(pos);
		// @TODO: get rid of magic values
		for (int i = 0x3000; i != 0x5400; i += 0x400, curr.x++)
		{
			curr.x = pos.x - C_RANGE_SMALL;
			for (int j = 0; j != 0x120; j += 0x20, curr.y++)
			{
				curr.y = pos.y - C_RANGE_SMALL;
				for (int k = 0; k != 9; k++, curr.z++)
				{
					curr.z = pos.z - C_RANGE_SMALL;

					TileID tile = level->getTile(curr);
					if (tile == Tile::treeTrunk->m_ID)
						field_70[0x18C + i + j + k] = 0;
					else if (tile == Tile::leaves->m_ID)
						field_70[0x18C + i + j + k] = -2;
					else
						field_70[0x18C + i + j + k] = -1;
				}
			}
		}

		constexpr int k1 = C_RANGE / 2;
		constexpr int j1 = C_RANGE * C_RANGE;

		for (int i2 = 1; i2 <= 4; i2++)
		{
			for (int l2 = -C_RANGE_SMALL; l2 <= C_RANGE_SMALL; l2++)
			{
				for (int j3 = -C_RANGE_SMALL; j3 <= C_RANGE_SMALL; j3++)
				{
					for (int l3 = -C_RANGE_SMALL; l3 <= C_RANGE_SMALL; l3++)
					{
						if (field_70[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] != i2 - 1)
							continue;

						if (field_70[((l2 + k1) - 1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] == -2)
							field_70[((l2 + k1) - 1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] = i2;

						if (field_70[(l2 + k1 + 1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] == -2)
							field_70[(l2 + k1 + 1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] = i2;

						if (field_70[(l2 + k1) * j1 + ((j3 + k1) - 1) * C_RANGE + (l3 + k1)] == -2)
							field_70[(l2 + k1) * j1 + ((j3 + k1) - 1) * C_RANGE + (l3 + k1)] = i2;

						if (field_70[(l2 + k1) * j1 + (j3 + k1 + 1) * C_RANGE + (l3 + k1)] == -2)
							field_70[(l2 + k1) * j1 + (j3 + k1 + 1) * C_RANGE + (l3 + k1)] = i2;

						if (field_70[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + ((l3 + k1) - 1)] == -2)
							field_70[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + ((l3 + k1) - 1)] = i2;

						if (field_70[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1 + 1)] == -2)
							field_70[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1 + 1)] = i2;
					}
				}
			}
		}

		if (field_70[0x4210] < 0)
			die(level, pos);
		else
			level->setDataNoUpdate(pos, data & ~0x4);
	}
}
