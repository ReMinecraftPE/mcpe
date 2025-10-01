/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LeafTile.hpp"
#include "world/level/Level.hpp"
#include "client/renderer/PatchManager.hpp"

#define C_REQUIRED_WOOD_RANGE 4
#define C_UPDATE_LEAF_BIT 8 // 4 on b1.2_02 & 0.1.0
#define C_NORMAL_LEAF 0
#define C_EVERGREEN_LEAF 1
#define C_BIRCH_LEAF 2
#define C_LEAF_TYPE_MASK 3

LeafTile::LeafTile(TileID id) : TransparentTile(id, TEXTURE_LEAVES_TRANSPARENT, Material::leaves, false)
{
	m_checkBuffer = nullptr;

	m_TextureFrame = TEXTURE_LEAVES_TRANSPARENT;
	field_74 = TEXTURE_LEAVES_TRANSPARENT;

	setTicking(true);
}

LeafTile::~LeafTile()
{
	if (m_checkBuffer)
		delete[] m_checkBuffer;
}

void LeafTile::_tickDecayOld(Level* level, const TilePos& pos)
{
	TileData data = level->getData(pos);
	if ((data & C_UPDATE_LEAF_BIT) == 0)
		return;
	
	constexpr int C_RANGE = 32;

	if (!m_checkBuffer)
		m_checkBuffer = new int[C_RANGE * C_RANGE * C_RANGE];

	if (level->hasChunksAt(pos - (C_REQUIRED_WOOD_RANGE + 1), pos + (C_REQUIRED_WOOD_RANGE + 1)))
	{
		TilePos curr(pos);
		// @TODO: get rid of magic values
		for (int i = 3 << 10; i != (5 << 12) + (1 << 10); i += 1 << 10, curr.x++)
		{
			curr.x = pos.x - C_REQUIRED_WOOD_RANGE;
			for (int j = 0; j != 9 << 5; j += C_RANGE, curr.y++)
			{
				curr.y = pos.y - C_REQUIRED_WOOD_RANGE;
				for (int k = 0; k != 9; k++, curr.z++)
				{
					curr.z = pos.z - C_REQUIRED_WOOD_RANGE;

					TileID tile = level->getTile(curr);
					if (tile == Tile::treeTrunk->m_ID)
						m_checkBuffer[0x18C + i + j + k] = 0;
					else if (tile == Tile::leaves->m_ID)
						m_checkBuffer[0x18C + i + j + k] = -2; // ~1
					else
						m_checkBuffer[0x18C + i + j + k] = -1; // ~0
				}
			}
		}

		constexpr int k1 = C_RANGE / 2;
		constexpr int j1 = C_RANGE * C_RANGE;

		for (int i2 = 1; i2 <= C_REQUIRED_WOOD_RANGE; i2++)
		{
			for (int l2 = -C_REQUIRED_WOOD_RANGE; l2 <= C_REQUIRED_WOOD_RANGE; l2++)
			{
				for (int j3 = -C_REQUIRED_WOOD_RANGE; j3 <= C_REQUIRED_WOOD_RANGE; j3++)
				{
					for (int l3 = -C_REQUIRED_WOOD_RANGE; l3 <= C_REQUIRED_WOOD_RANGE; l3++)
					{
						if (m_checkBuffer[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] != i2 - 1)
							continue;

						if (m_checkBuffer[((l2 + k1) - 1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] == -2)
							m_checkBuffer[((l2 + k1) - 1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] = i2;

						if (m_checkBuffer[(l2 + k1 + 1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] == -2)
							m_checkBuffer[(l2 + k1 + 1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] = i2;

						if (m_checkBuffer[(l2 + k1) * j1 + ((j3 + k1) - 1) * C_RANGE + (l3 + k1)] == -2)
							m_checkBuffer[(l2 + k1) * j1 + ((j3 + k1) - 1) * C_RANGE + (l3 + k1)] = i2;

						if (m_checkBuffer[(l2 + k1) * j1 + (j3 + k1 + 1) * C_RANGE + (l3 + k1)] == -2)
							m_checkBuffer[(l2 + k1) * j1 + (j3 + k1 + 1) * C_RANGE + (l3 + k1)] = i2;

						if (m_checkBuffer[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + ((l3 + k1) - 1)] == -2)
							m_checkBuffer[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + ((l3 + k1) - 1)] = i2;

						if (m_checkBuffer[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1 + 1)] == -2)
							m_checkBuffer[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1 + 1)] = i2;
					}
				}
			}
		}

		if (m_checkBuffer[0x4210] < 0)
			die(level, pos);
		else
			level->setDataNoUpdate(pos, data & ~C_UPDATE_LEAF_BIT); // equates to -5
	}
}

void LeafTile::_tickDecay(Level* level, const TilePos& pos)
{
	TileData data = level->getData(pos);
	if ((data & C_UPDATE_LEAF_BIT) == 0)
		return;

	constexpr int C_RANGE = 32;
	constexpr int k1 = C_RANGE / 2;
	constexpr int j1 = C_RANGE * C_RANGE;

	if (!m_checkBuffer)
		m_checkBuffer = new int[C_RANGE * C_RANGE * C_RANGE];

	if (level->hasChunksAt(pos - (C_REQUIRED_WOOD_RANGE + 1), pos + (C_REQUIRED_WOOD_RANGE + 1)))
	{
		TilePos curr(pos);
		for (int i2 = -C_REQUIRED_WOOD_RANGE; i2 <= C_REQUIRED_WOOD_RANGE; i2++)
		{
			curr.x = pos.x + i2;
			for (int j = -C_REQUIRED_WOOD_RANGE; j <= C_REQUIRED_WOOD_RANGE; j++)
			{
				curr.y = pos.y + j;
				for (int k = -C_REQUIRED_WOOD_RANGE; k <= C_REQUIRED_WOOD_RANGE; k++)
				{
					curr.z = pos.z + k;
					TileID tile = level->getTile(curr);
					m_checkBuffer[(i2 + k1) * j1 + (j + k1) * C_RANGE + k + k1] = tile == Tile::treeTrunk->m_ID ? 0 : tile == Tile::leaves->m_ID ? -2 : -1;
				}
			}
		}

		for (int i2 = 1; i2 <= C_REQUIRED_WOOD_RANGE; i2++)
		{
			for (int l2 = -C_REQUIRED_WOOD_RANGE; l2 <= C_REQUIRED_WOOD_RANGE; l2++)
			{
				for (int j3 = -C_REQUIRED_WOOD_RANGE; j3 <= C_REQUIRED_WOOD_RANGE; j3++)
				{
					for (int l3 = -C_REQUIRED_WOOD_RANGE; l3 <= C_REQUIRED_WOOD_RANGE; l3++)
					{
						if (m_checkBuffer[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] != i2 - 1)
							continue;

						if (m_checkBuffer[((l2 + k1) - 1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] == -2)
							m_checkBuffer[((l2 + k1) - 1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] = i2;

						if (m_checkBuffer[(l2 + k1 + 1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] == -2)
							m_checkBuffer[(l2 + k1 + 1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] = i2;

						if (m_checkBuffer[(l2 + k1) * j1 + ((j3 + k1) - 1) * C_RANGE + (l3 + k1)] == -2)
							m_checkBuffer[(l2 + k1) * j1 + ((j3 + k1) - 1) * C_RANGE + (l3 + k1)] = i2;

						if (m_checkBuffer[(l2 + k1) * j1 + (j3 + k1 + 1) * C_RANGE + (l3 + k1)] == -2)
							m_checkBuffer[(l2 + k1) * j1 + (j3 + k1 + 1) * C_RANGE + (l3 + k1)] = i2;

						if (m_checkBuffer[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + ((l3 + k1) - 1)] == -2)
							m_checkBuffer[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + ((l3 + k1) - 1)] = i2;

						if (m_checkBuffer[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1 + 1)] == -2)
							m_checkBuffer[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1 + 1)] = i2;
					}
				}
			}
		}
	}

	if (m_checkBuffer[k1 * j1 + k1 * C_RANGE + k1] < 0)
		die(level, pos);
	else
		level->setDataNoUpdate(pos, data & ~C_UPDATE_LEAF_BIT);
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

int LeafTile::getTexture(Facing::Name face, TileData data) const
{
	if ((data & C_LEAF_TYPE_MASK) == 1)
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

				level->setDataNoUpdate(pos + o, level->getData(pos + o) | C_UPDATE_LEAF_BIT);
			}
		}
	}
}

void LeafTile::tick(Level* level, const TilePos& pos, Random* random)
{
	if (level->m_bIsClientSide)
		return;

	_tickDecay(level, pos);
}

int LeafTile::getResource(TileData data, Random* random) const
{
	return random->nextInt(20) == 0 ? Tile::sapling->m_ID : 0;
}

int LeafTile::getSpawnResourcesAuxValue(int x) const
{
	return x & 3;
}
