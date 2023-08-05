/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Tile.hpp"
#include "world/level/Level.hpp"

LeafTile::LeafTile(int id) : TransparentTile(id, TEXTURE_LEAVES_TRANSPARENT, Material::leaves, false)
{
	m_TextureFrame = TEXTURE_LEAVES_TRANSPARENT;
	field_74 = TEXTURE_LEAVES_TRANSPARENT;

	setTicking(true);
}

LeafTile::~LeafTile()
{
	if (field_70)
		delete[] field_70;
}

void LeafTile::die(Level* level, int x, int y, int z)
{
	spawnResources(level, x, y, z, level->getData(x, y, z));
	level->setTile(x, y, z, TILE_AIR);
}

int LeafTile::getColor(LevelSource* level, int x, int y, int z)
{
#ifdef MOD_DONT_COLOR_GRASS
	return 0xffffff;
#endif
	return 0x339933;
}

int LeafTile::getTexture(int dir, int data)
{
	if ((data & 3) == 1)
		return m_TextureFrame + 80;

	return m_TextureFrame;
}

bool LeafTile::isSolidRender()
{
	return !m_bTransparent;
}

void LeafTile::stepOn(Level* level, int x, int y, int z, Entity* entity)
{
}

void LeafTile::onRemove(Level* level, int x, int y, int z)
{
	if (!level->hasChunksAt(x - 2, y - 2, z - 2, x + 2, y + 2, z + 2))
		return;

	for (int ox = -1; ox < 2; ox++)
	{
		for (int oy = -1; oy < 2; oy++)
		{
			for (int oz = -1; oz < 2; oz++)
			{
				TileID tile = level->getTile(x + ox, y + oy, z + oz);
				if (tile != Tile::leaves->m_ID) continue;

				level->setDataNoUpdate(x + ox, y + oy, z + oz, level->getData(x + ox, y + oy, z + oz) | 4);
			}
		}
	}
}

void LeafTile::tick(Level* level, int x, int y, int z, Random* random)
{
	if (level->field_11)
		return;

	int data = level->getData(x, y, z);
	if ((data & 4) == 0)
		return;

	constexpr int C_RANGE = 32;
	constexpr int C_RANGE_SMALL = 4;

	if (!field_70)
		field_70 = new int[C_RANGE * C_RANGE * C_RANGE];

	if (level->hasChunksAt(x - 5, y - 5, z - 5, x + 5, y + 5, z + 5))
	{
		// @TODO: get rid of magic values
		for (int currX = x - C_RANGE_SMALL, i = 0x3000; i != 0x5400; i += 0x400, currX++)
		{
			for (int currY = y - C_RANGE_SMALL, j = 0; j != 0x120; j += 0x20, currY++)
			{
				for (int currZ = z - C_RANGE_SMALL, k = 0; k != 9; k++, currZ++)
				{
					TileID tile = level->getTile(currX, currY, currZ);
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
			die(level, x, y, z);
		else
			level->setDataNoUpdate(x, y, z, data & ~0x4);
	}
}
