/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "common/Logger.hpp"
#include "world/level/Level.hpp"
#include "world/phys/AABB.hpp"

bool LevelChunk::touchedSky = false;

LevelChunk::~LevelChunk()
{
	SAFE_DELETE_ARRAY(m_lightBlk.m_data);
	SAFE_DELETE_ARRAY(m_lightSky.m_data);
	SAFE_DELETE_ARRAY(m_tileData.m_data);
}

constexpr int MakeBlockDataIndex(const ChunkTilePos& pos)
{
	return (pos.x << 11) | (pos.z << 7) | pos.y;
}

constexpr int MakeHeightMapIndex(const ChunkTilePos& pos)
{
	return pos.x | (pos.z * 16);
}

// Temp function for checking oob accesses into levelchunks
void CheckPosition(const ChunkTilePos& pos)
{
	assert(pos.x >= 0 && pos.y >= 0 && pos.z >= 0 && pos.x < 16 && pos.z < 16 && pos.y < 128);
}

void LevelChunk::_init()
{
	field_4 = 0;
	m_bLoaded = false;
	m_chunkPos = TilePos(0, 0, 0);
	field_234 = 0;
	m_bUnsaved = false;
	field_236 = 0;
	field_237 = 0;
	field_238 = 0;
	field_23C = 0;
	m_pBlockData = nullptr;
}

LevelChunk::LevelChunk(Level* pLevel, const ChunkPos& pos)
{
	_init();

	m_pLevel = pLevel;
	m_chunkPos = TilePos(pos, 0);

	init();
}

LevelChunk::LevelChunk(Level* pLevel, TileID* pData, const ChunkPos& pos)
{
	_init();

	m_pLevel = pLevel;

	// The game will blow its brains out if this if statement is executed
	// I have not the slightest idea as to why...
	/*if (pData)
	{*/
	field_4 = 16 * 16 * 128;
	m_tileData = DataLayer(16 * 16 * 128 / 2);
	//Space saving measure: Store 2 blocks' light field instead of only one block's, per byte.
	m_lightSky = DataLayer(16 * 16 * 128 / 2);
	m_lightBlk = DataLayer(16 * 16 * 128 / 2);

	m_pBlockData = pData;
	//}

	m_chunkPos = TilePos(pos, 0);

	init();
}

void LevelChunk::init()
{
	field_234 = 0;
	m_bUnsaved = false;
	field_237 = 0;
	field_238 = 0;
	field_23C = 0;
	memset(m_heightMap, 0, sizeof m_heightMap);
	memset(m_updateMap, 0, sizeof m_updateMap);
}

void LevelChunk::unload()
{
	m_bLoaded = false;
}

bool LevelChunk::isAt(const ChunkPos& pos)
{
	return m_chunkPos == pos;
}

int LevelChunk::getHeightmap(const ChunkTilePos& pos)
{
	CheckPosition(pos);
	return m_heightMap[MakeHeightMapIndex(pos)];
}

void LevelChunk::recalcHeightmap()
{
	int x1 = 127;

	ChunkTilePos pos(0, 0, 0);

	for (pos.x = 0; pos.x < 16; pos.x++)
	{
		for (pos.z = 0; pos.z < 16; pos.z++)
		{
			int index1 = MakeBlockDataIndex(pos);
			int index2 = 126;

			int x2 = 0;
			while (!Tile::lightBlock[m_pBlockData[index1 + index2]])
			{
				if (index2-- == 0)
				{
					x2 = 0;
					goto label_7;
				}
			}

			x2 = index2 + 1;
		label_7:
			if (x1 >= x2)
				x1 = x2;
			m_heightMap[MakeHeightMapIndex(pos)] = x2;

			if (!m_pLevel->m_pDimension->field_E)
			{
				int x4 = 15;
				for (int x3 = 127; x3 > 0; x3--)
				{
					x4 -= Tile::lightBlock[m_pBlockData[index1 + x3]];
					if (x4 <= 0)
						break;

					m_lightSky.set(ChunkTilePos(pos.x, x3, pos.z), x4);
				}
			}
		}
	}

	field_228 = x1;

	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			lightGaps(ChunkTilePos(i, 0, j));
		}
	}
}

void LevelChunk::recalcHeightmapOnly()
{
	int x1 = 127;

	ChunkTilePos pos(0, 0, 0);
	for (pos.x = 0; pos.x < 16; pos.x++)
	{
		for (pos.z = 0; pos.z < 16; pos.z++)
		{
			int index1 = MakeBlockDataIndex(pos);
			int index2 = 126;

			int x2 = 0;
			while (!Tile::lightBlock[m_pBlockData[index1 + index2]])
			{
				if (index2-- == 0)
				{
					x2 = 0;
					goto label_7;
				}
			}

			x2 = index2 + 1;
		label_7:
			if (x1 >= x2)
				x1 = x2;
			m_heightMap[MakeHeightMapIndex(pos)] = x2;
		}
	}

	field_228 = x1;
}

void LevelChunk::lightGaps(const ChunkTilePos& pos)
{
	ChunkTilePos coords = pos + m_chunkPos;
	CheckPosition(pos);
	uint8_t heightMap = getHeightmap(pos);
	coords.y = heightMap;

	// @TODO: coords.direction()
	lightGap(TilePos(coords.x - 1, coords.y, coords.z), heightMap);
	lightGap(TilePos(coords.x + 1, coords.y, coords.z), heightMap);
	lightGap(TilePos(coords.x, coords.y, coords.z - 1), heightMap);
	lightGap(TilePos(coords.x, coords.y, coords.z + 1), heightMap);
}

void LevelChunk::lightGap(const TilePos& pos, uint8_t heightMap)
{
	// @BUG: This is flawed. getHeightmap calls getChunk which creates another chunk and calls this over
	// and over again, creating a stack overflow. Since the level is limited a stack overflow is only
	// possible on devices with really tight stacks.
	uint8_t currHeightMap = m_pLevel->getHeightmap(pos);
	if (currHeightMap > heightMap)
	{
		m_pLevel->updateLight(LightLayer::Sky, TilePos(pos.x, heightMap, pos.z), TilePos(pos.x, currHeightMap, pos.z));
		return;
	}
	if (currHeightMap < heightMap)
	{
		m_pLevel->updateLight(LightLayer::Sky, TilePos(pos.x, currHeightMap, pos.z), TilePos(pos.x, heightMap, pos.z));
		return;
	}
}

int LevelChunk::getBrightness(const LightLayer& ll, const ChunkTilePos& pos)
{
	CheckPosition(pos);

	// why the hell is it doing it like that.
	if (&ll == &LightLayer::Sky)
	{
		return m_lightSky.get(pos);
	}

	if (&ll == &LightLayer::Block)
	{
		return m_lightBlk.get(pos);
	}

	return 0;
}

void LevelChunk::setBrightness(const LightLayer& ll, const ChunkTilePos& pos, int brightness)
{
	CheckPosition(pos);
	// why the hell is it doing it like that.
	if (&ll == &LightLayer::Sky)
	{
		m_lightSky.set(pos, brightness);

		return;
	}

	if (&ll == &LightLayer::Block)
	{
		m_lightBlk.set(pos, brightness);

		return;
	}
}

int LevelChunk::getRawBrightness(const ChunkTilePos& pos, int skySubtract)
{
	CheckPosition(pos);
	int bitIdx = MakeBlockDataIndex(pos);
	int index = bitIdx >> 1, offs = bitIdx & 1;

	uint8_t bSky = m_lightSky.get(pos);
	if (bSky > 0)
		touchedSky = true;

	int br = bSky - skySubtract;

	uint8_t bBlk = m_lightBlk.get(pos);
	// if it's smaller than 0 it'll probably sort itself out
	if (br < bBlk)
		br = bBlk;

	return br;
}

void LevelChunk::addEntity(Entity* pEnt)
{
	assert(pEnt != nullptr); // Cannot add a null entity
	field_238 = 1;

	int yCoord = int(floorf(pEnt->m_pos.y / 16));
	if (yCoord < 0) yCoord = 0;
	if (yCoord > 7) yCoord = 7;
	pEnt->m_bInAChunk = true;
	pEnt->m_chunkPos = m_chunkPos;
	pEnt->m_chunkPosY = yCoord;

	m_entities[yCoord].push_back(pEnt);
}

void LevelChunk::clearUpdateMap()
{
	memset(m_updateMap, 0, sizeof m_updateMap);
	m_bUnsaved = 0;
}

void LevelChunk::deleteBlockData()
{
	if (m_pBlockData)
		delete[] m_pBlockData;

	m_pBlockData = nullptr;
}

void LevelChunk::removeEntity(Entity* pEnt)
{
	removeEntity(pEnt, pEnt->m_chunkPosY);
}

void LevelChunk::removeEntity(Entity* pEnt, int vec)
{
	if (vec < 0) vec = 0;
	if (vec > 7) vec = 7;

	std::vector<Entity*>::iterator it = std::find(m_entities[vec].begin(), m_entities[vec].end(), pEnt);

	if (it != m_entities[vec].end())
		m_entities[vec].erase(it);
	/*else
		assert(false);*/ // The chunk doesn't contain this entity
}

bool LevelChunk::isSkyLit(const ChunkTilePos& pos)
{
	CheckPosition(ChunkTilePos(pos.x, 64, pos.z));

	return m_heightMap[MakeHeightMapIndex(pos)] <= pos.y;
}

void LevelChunk::lightLava()
{

}

void LevelChunk::recalcBlockLights()
{

}

void LevelChunk::recalcHeight(const ChunkTilePos& pos)
{
	CheckPosition(ChunkTilePos(pos.x, 64, pos.z));

	uint8_t& hmap = m_heightMap[MakeHeightMapIndex(pos)];

	int index = MakeBlockDataIndex(ChunkTilePos(pos.x, 0, pos.z));

	int x1 = hmap;
	if (x1 < pos.y)
		x1 = pos.y;

	if (x1 > 0)
	{
		while (!Tile::lightBlock[m_pBlockData[x1 - 1 + index]] && x1)
		{
			--x1;
		}
	}

	if (x1 != hmap)
	{
		m_pLevel->lightColumnChanged(pos.x, pos.z, x1, hmap);
		hmap = x1;
		if (field_228 <= x1)
		{
			int x6 = 127;
			ChunkTilePos pos(0, 0, 0);
			for (pos.x = 0; pos.x < 16; pos.x++)
			{
				for (pos.z = 0; pos.z < 16; pos.z++)
				{
					int x = m_heightMap[MakeHeightMapIndex(pos)];
					if (x6 >= x)
						x6 = x;
				}
			}

			field_228 = x6;
		}
		else
		{
			field_228 = x1;
		}

		int globalX = pos.x + 16 * m_chunkPos.x;
		int globalZ = pos.z + 16 * m_chunkPos.z;
		if (hmap <= x1)
		{
			m_pLevel->updateLight(LightLayer::Sky, TilePos(globalX, hmap, globalZ), TilePos(globalX, x1, globalZ));

			if (hmap < x1)
			{
				for (int i = 0; i < x1 - hmap; i++)
				{
					m_lightSky.set(ChunkTilePos(pos.x, i, pos.z), 0);
				}
			}
		}
		else
		{
			for (int i = 0; i < hmap; i++)
			{
				m_lightSky.set(ChunkTilePos(pos.x, i, pos.z), 15);
				}
				}

		int x2 = x1;
		int x3 = 15;
		while (x3 > 0 && x2 > 0)
		{
			TileID tile = getTile(ChunkTilePos(pos.x, --x2, pos.z));

			int x4 = Tile::lightBlock[tile];
			if (!x4)
				x4 = 1; //@HUH: what is this?

			int x5 = x3 - x4;

			x3 = x5;
			if (x3 < 0)
				x3 = 0;

			m_lightSky.set(ChunkTilePos(pos.x, x2, pos.z), x3);
		}

		if (x2 > 0)
		{
			for (; x2 > 0; x2--)
			{
				if (Tile::lightBlock[getTile(ChunkTilePos(pos.x, x2 - 1, pos.z))])
					break;
			}
		}

		if (x2 != x1)
		{
			m_pLevel->updateLight(LightLayer::Sky, TilePos(globalX - 1, x2, globalZ - 1), TilePos(globalX + 1, x1, globalZ + 1));
		}
	}
}

void LevelChunk::skyBrightnessChanged()
{

}

void LevelChunk::load()
{
	m_bLoaded = true;
}

bool LevelChunk::shouldSave(bool b)
{
	if (field_236)
		return false;

	if (!b)
	{
		if (field_238)
		{
			if (field_23C + 599 < m_pLevel->getTime())
				return true;
		}

		return m_bUnsaved;
	}

	if (!field_238 || m_pLevel->getTime() == field_23C)
		return m_bUnsaved;

	return true;
}

void LevelChunk::markUnsaved()
{
	m_bUnsaved = true;
}

TileID LevelChunk::getTile(const ChunkTilePos& pos)
{
	CheckPosition(pos);

	TileID tileId = m_pBlockData[MakeBlockDataIndex(pos)];
	if (Tile::tiles[tileId])
		return tileId;
	else
		return TILE_AIR;
}

int LevelChunk::countEntities()
{
	int n = 0;
	for (int i = 0; i < 8; i++)
		n += int(m_entities[i].size());
	return n;
}

void LevelChunk::getEntities(Entity* pEntExclude, const AABB& aabb, std::vector<Entity*>& out)
{
	int lowerBound = int(floorf((aabb.min.y - 2.0f) / 16.0f));
	int upperBound = int(floorf((aabb.max.y + 2.0f) / 16.0f));

	if (lowerBound < 0) lowerBound = 0;
	if (upperBound > 7) upperBound = 7;

	for (int b = lowerBound; b <= upperBound; b++)
	{
		for (std::vector<Entity*>::iterator it = m_entities[b].begin(); it != m_entities[b].end(); it++)
		{
			Entity* ent = *it;
			if (ent == pEntExclude) continue;

			if (!aabb.intersect(ent->m_hitbox)) continue;

			out.push_back(ent);
		}
	}
}

bool LevelChunk::setTile(const ChunkTilePos& pos, TileID tile)
{
	CheckPosition(pos);

	int index = MakeBlockDataIndex(pos);

	TileID oldTile = m_pBlockData[index];

	uint8_t height = m_heightMap[MakeHeightMapIndex(pos)];

	if (oldTile == tile)
		return false;

	TilePos tilePos(m_chunkPos, pos.y);
	tilePos.x += pos.x;
	tilePos.z += pos.z;
	m_pBlockData[index] = tile;
	if (oldTile)
	{
		Tile::tiles[oldTile]->onRemove(m_pLevel, tilePos);
	}

	// clear the data value of the block
	m_tileData.set(pos, 0);

	if (Tile::lightBlock[tile])
	{
		if (height <= pos.y)
			recalcHeight(ChunkTilePos(pos.x, pos.y + 1, pos.z));
	}
	else if (height - 1 == pos.y)
	{
		recalcHeight(pos);
	}

	m_pLevel->updateLight(LightLayer::Sky, tilePos, tilePos);
	m_pLevel->updateLight(LightLayer::Block, tilePos, tilePos);

	lightGaps(pos);
	if (tile)
	{
		if (!m_pLevel->m_bIsClientSide)
			Tile::tiles[tile]->onPlace(m_pLevel, tilePos);
	}

	m_bUnsaved = true;
	m_updateMap[MakeHeightMapIndex(pos)] |= 1 << (pos.y >> 4);

	return true;
}

bool LevelChunk::setTileAndData(const ChunkTilePos& pos, TileID tile, TileData data)
{
	CheckPosition(pos);

	int index = MakeBlockDataIndex(pos);

	TileID oldTile = m_pBlockData[index];

	uint8_t height = m_heightMap[MakeHeightMapIndex(pos)];

	if (oldTile == tile)
	{
		// make sure we're at least updating the data. If not, simply return false
		if (getData(pos) == data)
			return false;
	}

	TilePos tilePos(m_chunkPos, pos.y);
	tilePos.x += pos.x;
	tilePos.z += pos.z;
	m_pBlockData[index] = tile;
	if (oldTile)
	{
		Tile::tiles[oldTile]->onRemove(m_pLevel, tilePos);
	}

	// update the data value of the block
	m_tileData.set(pos, data);

	if (m_pLevel->m_pDimension->field_E)
	{
		m_pLevel->updateLight(LightLayer::Block, tilePos, tilePos);
		lightGaps(pos);
	}

	if (Tile::lightBlock[tile])
	{
		if (height <= pos.y)
			recalcHeight(ChunkTilePos(pos.x, pos.y + 1, pos.z));
	}
	else if (height - 1 == pos.y)
	{
		recalcHeight(pos);
	}

	m_pLevel->updateLight(LightLayer::Sky, tilePos, tilePos);
	m_pLevel->updateLight(LightLayer::Block, tilePos, tilePos);

	lightGaps(pos);
	if (tile)
	{
		if (!m_pLevel->m_bIsClientSide)
			Tile::tiles[tile]->onPlace(m_pLevel, tilePos);
	}

	m_bUnsaved = true;
	m_updateMap[MakeHeightMapIndex(pos)] |= 1 << (pos.y >> 4);

	return true;
}

TileData LevelChunk::getData(const ChunkTilePos& pos)
{
	CheckPosition(pos);

	return m_tileData.get(pos);
}

bool LevelChunk::setData(const ChunkTilePos& pos, TileData data)
{
	CheckPosition(pos);

	if (m_tileData.get(pos) == data)
		return false;

	m_tileData.set(pos, data);

	return true;
}

// seems to set block data in 8192 block (4*16*128) chunks for some reason ?
// This function appears to be unused, and is completely removed as of 0.9.2
void LevelChunk::setBlocks(uint8_t* pData, int y)
{
	LOG_I("LevelChunk::setBlocks");
	for (int i = 0; i < 8192; i++)
	{
		m_pBlockData[8192 * y + i] = pData[i];
	}

	int x1 = 4 * y;
	int x2 = 4 * (y + 1);
	if (4 * y < x2)
	{
		for (int x3 = 4 * y; x3 < x2; x3++)
		{
			for (int j = 0; j < 16; j++)
			{
				recalcHeight(ChunkTilePos(x3, 0, j));
			}
		}
	}

	TilePos tilePos(m_chunkPos, 0);
	tilePos.x += x1;
	TilePos tilePos2 = tilePos;
	tilePos.x += 4;
	tilePos.y = 128;
	tilePos.x += 16;

	m_pLevel->updateLight(LightLayer::Sky, tilePos, tilePos2);
	m_pLevel->updateLight(LightLayer::Block, tilePos, tilePos2);
	m_pLevel->setTilesDirty(tilePos, tilePos2);
}

// This function appears to be unused, and is completely removed as of 0.9.2
int LevelChunk::setBlocksAndData(uint8_t* pData, int a3, int a4, int a5, int a6, int a7, int a8, int a9)
{
	LOG_I("LevelChunk::setBlocksAndData");

	if (a3 >= a6)
	{
		recalcHeightmapOnly();
		return a9;
	}

	// load the tile IDs
	int x2 = a7 - a4;
	for (int x1 = a3; x1 < a6; x1++)
	{
		if (a5 >= a8) continue;

		uint8_t* src = &pData[a9];

		for (int x3 = a5; x3 < a8; x3++)
		{
			uint8_t* dst = &m_pBlockData[MakeBlockDataIndex(ChunkTilePos(x1, a4, x3))];
			memcpy(dst, src, x2 * sizeof(TileID));
			src += x2 * sizeof(TileID);
			a9 += x2 * sizeof(TileID);
		}
	}

	recalcHeightmapOnly();

	int x5 = x2 / 2;

	// load the tile data
	for (int x1 = a3; x1 < a6; x1++)
	{
		if (a5 >= a8) continue;

		uint8_t* src = &pData[a9];

		for (int x3 = a5; x3 < a8; x3++)
		{
			uint8_t* dst = &m_tileData.m_data[MakeBlockDataIndex(ChunkTilePos(x1, a4, x3)) >> 1];
			memcpy(dst, src, x5);
			src += x5;
			a9 += x5;
		}
	}

	// load the block lights
	for (int x1 = a3; x1 < a6; x1++)
	{
		if (a5 >= a8) continue;

		uint8_t* src = &pData[a9];

		for (int x3 = a5; x3 < a8; x3++)
		{
			uint8_t* dst = &m_lightBlk.m_data[MakeBlockDataIndex(ChunkTilePos(x1, a4, x3)) >> 1];
			memcpy(dst, src, x5);
			src += x5;
			a9 += x5;
		}
	}

	// load the sky lights
	for (int x1 = a3; x1 < a6; x1++)
	{
		if (a5 >= a8) continue;

		uint8_t* src = &pData[a9];

		for (int x3 = a5; x3 < a8; x3++)
		{
			uint8_t* dst = &m_lightSky.m_data[MakeBlockDataIndex(ChunkTilePos(x1, a4, x3)) >> 1];
			memcpy(dst, src, x5);
			src += x5;
			a9 += x5;
		}
	}

	return a9;
}

// This function appears to be unused, and is completely removed as of 0.9.2
int LevelChunk::getBlocksAndData(uint8_t* pData, int a3, int a4, int a5, int a6, int a7, int a8, int a9)
{
	if (a3 >= a6)
	{
		recalcHeightmapOnly();
		return a9;
	}

	// load the tile IDs
	int x2 = a7 - a4;
	for (int x1 = a3; x1 < a6; x1++)
	{
		if (a5 >= a8) continue;

		uint8_t* dst = &pData[a9];

		for (int x3 = a5; x3 < a8; x3++)
		{
			uint8_t* src = &m_pBlockData[MakeBlockDataIndex(ChunkTilePos(x1, a4, x3))];
			memcpy(dst, src, x2 * sizeof(TileID));
			dst += x2 * sizeof(TileID);
			a9 += x2 * sizeof(TileID);
		}
	}

	recalcHeightmapOnly();

	int x5 = x2 / 2;

	// load the tile data
	for (int x1 = a3; x1 < a6; x1++)
	{
		if (a5 >= a8) continue;

		uint8_t* dst = &pData[a9];

		for (int x3 = a5; x3 < a8; x3++)
		{
			uint8_t* src = &m_tileData.m_data[MakeBlockDataIndex(ChunkTilePos(x1, a4, x3)) >> 1];
			memcpy(dst, src, x5);
			dst += x5;
			a9 += x5;
		}
	}

	// load the block lights
	for (int x1 = a3; x1 < a6; x1++)
	{
		if (a5 >= a8) continue;

		uint8_t* dst = &pData[a9];

		for (int x3 = a5; x3 < a8; x3++)
		{
			uint8_t* src = &m_lightBlk.m_data[MakeBlockDataIndex(ChunkTilePos(x1, a4, x3)) >> 1];
			memcpy(dst, src, x5);
			dst += x5;
			a9 += x5;
		}
	}

	// load the sky lights
	for (int x1 = a3; x1 < a6; x1++)
	{
		if (a5 >= a8) continue;

		uint8_t* dst = &pData[a9];

		for (int x3 = a5; x3 < a8; x3++)
		{
			uint8_t* src = &m_lightSky.m_data[MakeBlockDataIndex(ChunkTilePos(x1, a4, x3)) >> 1];
			memcpy(dst, src, x5);
			dst += x5;
			a9 += x5;
		}
	}

	return a9;
}

// This function appears to be unused, and is completely removed as of 0.9.2
Random LevelChunk::getRandom(int32_t l)
{
	Random random;

	int levelSeed = m_pLevel->getSeed();
	int chunkSeed = m_chunkPos.x * (4987142 * m_chunkPos.x + 5947611) + m_chunkPos.z * (4392871 * m_chunkPos.z + 389711);

	random.init_genrand((levelSeed + chunkSeed) ^ l);

	return random;
}

bool LevelChunk::isEmpty()
{
	return false;
}
