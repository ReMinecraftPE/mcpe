/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Level.hpp"

bool LevelChunk::touchedSky = false;

LevelChunk::~LevelChunk()
{
	SAFE_DELETE(m_lightBlk);
	SAFE_DELETE(m_lightSky);
	SAFE_DELETE(m_tileData);
}

constexpr int MakeBlockDataIndex (int x, int y, int z)
{
	return (x << 11) | (z << 7) | y;
}

constexpr int MakeHeightMapIndex(int x, int z)
{
	return x | (z << 4);
}

// Temp function for checking oob accesses into levelchunks
void CheckPosition(int x, int y, int z)
{
	assert(x >= 0 && y >= 0 && z >= 0 && x < 16 && z < 16 && y < 128);
}

LevelChunk::LevelChunk(Level* pLevel, int x, int z)
{
	m_pLevel = pLevel;
	m_chunkX = x;
	m_chunkZ = z;

	init();
}

LevelChunk::LevelChunk(Level* pLevel, TileID* pData, int x, int z)
{
	m_pLevel = pLevel;

	m_tileDataCnt = 0x4000;
	field_4 = 0x8000;
	m_tileData = new uint8_t[m_tileDataCnt];
	memset(m_tileData, 0, m_tileDataCnt);

	//Space saving measure: Store 2 blocks' light field instead of only one block's, per byte.
	m_lightSkyCnt = 16 * 16 * 128 / 2;
	m_lightSky = new uint8_t[m_lightSkyCnt];
	memset(m_lightSky, 0, m_lightSkyCnt);

	m_lightBlkCnt = 16 * 16 * 128 / 2;
	m_lightBlk = new uint8_t[m_lightBlkCnt];
	memset(m_lightBlk, 0, m_lightBlkCnt);

	m_pBlockData = pData;
	m_chunkX = x;
	m_chunkZ = z;

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

bool LevelChunk::isAt(int x, int z)
{
	return m_chunkX == x && m_chunkZ == z;
}

int LevelChunk::getHeightmap(int x, int z)
{
	CheckPosition(x, 64, z);
	return m_heightMap[MakeHeightMapIndex(x, z)];
}

void LevelChunk::recalcHeightmap()
{
	int x1 = 127;

	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			int index1 = MakeBlockDataIndex(i, 0, j);
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
			m_heightMap[MakeHeightMapIndex(i, j)] = x2;

			if (!m_pLevel->m_pDimension->field_E)
			{
				int x4 = 15;
				for (int x3 = 127; x3 > 0; x3--)
				{
					x4 -= Tile::lightBlock[m_pBlockData[index1 + x3]];
					if (x4 <= 0)
						break;

					int x = x3 + index1;
					int index = x >> 1, offs = x & 1;

					if (offs)
						m_lightSky[index] = (m_lightSky[index] & 0x0F) | (x4 << 4); // set the upper 4 bits to x4
					else
						m_lightSky[index] = (m_lightSky[index] & 0xF0) | x4; // set the lower 4 bits to x4
				}
			}
		}
	}

	field_228 = x1;

	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			lightGaps(i, j);
		}
	}
}

void LevelChunk::recalcHeightmapOnly()
{
	int x1 = 127;

	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			int index1 = MakeBlockDataIndex(i, 0, j);
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
			m_heightMap[MakeHeightMapIndex(i, j)] = x2;
		}
	}

	field_228 = x1;
}

void LevelChunk::lightGaps(int x, int z)
{
	CheckPosition(x, 64, z);
	uint8_t heightMap = getHeightmap(x, z);

	int coordX = x + 16 * m_chunkX;
	int coordZ = z + 16 * m_chunkZ;

	lightGap(coordX - 1, coordZ, heightMap);
	lightGap(coordX + 1, coordZ, heightMap);
	lightGap(coordX, coordZ - 1, heightMap);
	lightGap(coordX, coordZ + 1, heightMap);
}

void LevelChunk::lightGap(int gx, int gz, uint8_t heightMap)
{
	// @BUG: This is flawed. getHeightmap calls getChunk which creates another chunk and calls this over
	// and over again, creating a stack overflow. Since the level is limited a stack overflow is only
	// possible on devices with really tight stacks.
	uint8_t currHeightMap = m_pLevel->getHeightmap(gx, gz);
	if (currHeightMap > heightMap)
	{
		m_pLevel->updateLight(LightLayer::Sky, gx, heightMap, gz, gx, currHeightMap, gz);
		return;
	}
	if (currHeightMap < heightMap)
	{
		m_pLevel->updateLight(LightLayer::Sky, gx, currHeightMap, gz, gx, heightMap, gz);
		return;
	}
}

int LevelChunk::getBrightness(const LightLayer& ll, int x, int y, int z)
{
	CheckPosition(x, y, z);

	// why the hell is it doing it like that.
	if (&ll == &LightLayer::Sky)
	{
		int bitIdx = MakeBlockDataIndex(x, y, z);
		int index = bitIdx >> 1, offs = bitIdx & 1;
		if (offs)
			return m_lightSky[index] >> 4;
		else
			return m_lightSky[index] & 0xF;
	}

	if (&ll == &LightLayer::Block)
	{
		int bitIdx = MakeBlockDataIndex(x, y, z);
		int index = bitIdx >> 1, offs = bitIdx & 1;
		if (offs)
			return m_lightBlk[index] >> 4;
		else
			return m_lightBlk[index] & 0xF;
	}

	return 0;
}

void LevelChunk::setBrightness(const LightLayer& ll, int x, int y, int z, int brightness)
{
	CheckPosition(x, y, z);
	// why the hell is it doing it like that.
	if (&ll == &LightLayer::Sky)
	{
		int bitIdx = MakeBlockDataIndex(x, y, z);
		int index = bitIdx >> 1, offs = bitIdx & 1;
		if (offs)
			m_lightSky[index] = (m_lightSky[index] & 0x0F) | (brightness << 4);
		else
			m_lightSky[index] = (m_lightSky[index] & 0xF0) | (brightness & 0xF);

		return;
	}

	if (&ll == &LightLayer::Block)
	{
		int bitIdx = MakeBlockDataIndex(x, y, z);
		int index = bitIdx >> 1, offs = bitIdx & 1;
		if (offs)
			m_lightBlk[index] = (m_lightBlk[index] & 0x0F) | (brightness << 4);
		else
			m_lightBlk[index] = (m_lightBlk[index] & 0xF0) | (brightness & 0xF);

		return;
	}
}

int LevelChunk::getRawBrightness(int x, int y, int z, int skySubtract)
{
	CheckPosition(x, y, z);
	int bitIdx = MakeBlockDataIndex(x, y, z);
	int index = bitIdx >> 1, offs = bitIdx & 1;

	uint8_t bSky, bBlk;
	if (offs)
		bSky = m_lightSky[index] >> 4;
	else
		bSky = m_lightSky[index] & 0xF;

	if (m_lightSky)
		touchedSky = true;

	int br = bSky - skySubtract;

	if (offs)
		bBlk = m_lightBlk[index] >> 4;
	else
		bBlk = m_lightBlk[index] & 0xF;

	// if it's smaller than 0 it'll probably sort itself out
	if (br < bBlk)
		br = bBlk;
	
	return br;
}

void LevelChunk::addEntity(Entity* pEnt)
{
	field_238 = 1;

	int yCoord = int(floorf(pEnt->m_pos.y / 16));
	if (yCoord < 0) yCoord = 0;
	if (yCoord > 7) yCoord = 7;
	pEnt->m_bInAChunk = true;
	pEnt->m_chunkX = m_chunkX;
	pEnt->m_chunkY = yCoord;
	pEnt->m_chunkZ = m_chunkZ;

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
	removeEntity(pEnt, pEnt->m_chunkY);
}

void LevelChunk::removeEntity(Entity* pEnt, int vec)
{
	if (vec < 0) vec = 0;
	if (vec > 7) vec = 7;

	auto it = std::find(m_entities[vec].begin(), m_entities[vec].end(), pEnt);

	if (it != m_entities[vec].end())
		m_entities[vec].erase(it);
}

bool LevelChunk::isSkyLit(int x, int y, int z)
{
	CheckPosition(x, 64, z);

	return m_heightMap[MakeHeightMapIndex(x, z)] <= y;
}

void LevelChunk::lightLava()
{

}

void LevelChunk::recalcBlockLights()
{

}

void LevelChunk::recalcHeight(int x, int y, int z)
{
	CheckPosition(x, 64, z);

	uint8_t& hmap = m_heightMap[MakeHeightMapIndex(x, z)];

	int index = MakeBlockDataIndex(x, 0, z);

	int x1 = hmap;
	if (x1 < y)
		x1 = y;

	if (x1 > 0)
	{
		while (!Tile::lightBlock[m_pBlockData[x1 - 1 + index]] && x1)
		{
			--x1;
		}
	}

	if (x1 != hmap)
	{
		m_pLevel->lightColumnChanged(x, z, x1, hmap);
		hmap = x1;
		if (field_228 <= x1)
		{
			int x6 = 127;
			for (int j = 0; j < 16; j++)
			{
				for (int i = 0; i < 16; i++)
				{
					int x = m_heightMap[MakeHeightMapIndex(j, i)];
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

		int globalX = x + 16 * m_chunkX;
		int globalZ = z + 16 * m_chunkZ;
		if (hmap <= x1)
		{
			m_pLevel->updateLight(LightLayer::Sky, globalX, hmap, globalZ, globalX, x1, globalZ);

			if (hmap < x1)
			{
				for (int i = 0; i < x1 - hmap; i++)
				{
					int bitIdx = index | (i + hmap);
					int _idx = bitIdx >> 1;
					int _off = bitIdx & 1;

					if (_off)
						m_lightSky[_idx] &= 0xF0;
					else
						m_lightSky[_idx] &= 0x0F;
				}
			}
		}
		else
		{
			for (int i = 0; i < hmap; i++)
			{
				int v15 = (i | index) >> 1;
				int v16 = (i | index) <<31;

				if ((i | index) & 1)
				{
					m_lightSky[v15] = (m_lightSky[v15] & 0xF0) | 0x0F;
				}
				else
				{
					m_lightSky[v15] = (m_lightSky[v15] & 0x0F) | 0xF0;
				}
			}
		}

		int x2 = x1;
		int x3 = 15;
		while (x3 > 0 && x2 > 0)
		{
			TileID tile = getTile(x, --x2, z);
			int bitIdx = x2 | index;

			int x4 = Tile::lightBlock[tile];
			if (!x4)
				x4 = 1; //@HUH: what is this?

			int x5 = x3 - x4;
			int _idx = bitIdx >> 1;
			int _off = (bitIdx & 1);

			x3 = x5;
			if (x3 < 0)
				x3 = 0;

			if (_off)
				m_lightSky[_idx] = (m_lightSky[_idx] & 0x0F) | ((x3) << 4);
			else
				m_lightSky[_idx] = (m_lightSky[_idx] & 0xF0) | x3;
		}

		if (x2 > 0)
		{
			for (; x2 > 0; x2--)
			{
				if (Tile::lightBlock[getTile(x, x2 - 1, z)])
					break;
			}
		}

		if (x2 != x1)
		{
			m_pLevel->updateLight(LightLayer::Sky, globalX - 1, x2, globalZ - 1, globalX + 1, x1, globalZ + 1);
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

TileID LevelChunk::getTile(int x, int y, int z)
{
	CheckPosition(x, y, z);
	
	return m_pBlockData[MakeBlockDataIndex(x, y, z)];
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
		for (auto ent : m_entities[b])
		{
			if (ent == pEntExclude) continue;
			
			if (!aabb.intersect(ent->m_hitbox)) continue;

			out.push_back(ent);
		}
	}
}

bool LevelChunk::setTile(int x, int y, int z, TileID tile)
{
	CheckPosition(x, y, z);

	int index = MakeBlockDataIndex(x, y, z);

	TileID oldTile = m_pBlockData[index];

	uint8_t height = m_heightMap[MakeHeightMapIndex(x, z)];

	if (oldTile == tile)
		return false;

	int globalX = x + 16 * m_chunkX;
	int globalZ = z + 16 * m_chunkZ;
	m_pBlockData[index] = tile;
	if (oldTile)
	{
		Tile::tiles[oldTile]->onRemove(m_pLevel, globalX, y, globalZ);
	}

	// clear the data value of the block
	if (index & 1)
		m_tileData[index >> 1] &= 0xF;
	else
		m_tileData[index >> 1] &= 0xF0;

	if (Tile::lightBlock[tile])
	{
		if (height <= y)
			recalcHeight(x, y + 1, z);
	}
	else if (height - 1 == y)
	{
		recalcHeight(x, y, z);
	}

	m_pLevel->updateLight(LightLayer::Sky,   globalX, y, globalZ, globalX, y, globalZ);
	m_pLevel->updateLight(LightLayer::Block, globalX, y, globalZ, globalX, y, globalZ);

	lightGaps(x, z);
	if (tile)
	{
		if (!m_pLevel->field_11)
			Tile::tiles[tile]->onPlace(m_pLevel, globalX, y, globalZ);
	}

	m_bUnsaved = true;
	m_updateMap[MakeHeightMapIndex(x, z)] |= 1 << (y >> 4);

	return true;
}

bool LevelChunk::setTileAndData(int x, int y, int z, TileID tile, int data)
{
	CheckPosition(x, y, z);

	assert((data & ~0xF) == 0);
	data &= 0xF;

	int index = MakeBlockDataIndex(x, y, z);

	TileID oldTile = m_pBlockData[index];

	uint8_t height = m_heightMap[MakeHeightMapIndex(x, z)];

	if (oldTile == tile)
	{
		// make sure we're at least updating the data. If not, simply return false
		if (getData(x, y, z) == data)
			return false;
	}

	int globalX = x + 16 * m_chunkX;
	int globalZ = z + 16 * m_chunkZ;
	m_pBlockData[index] = tile;
	if (oldTile)
	{
		Tile::tiles[oldTile]->onRemove(m_pLevel, globalX, y, globalZ);
	}

	// update the data value of the block
	if (index & 1)
		m_tileData[index >> 1] = (m_tileData[index >> 1] & 0x0F) | (data << 4);
	else
		m_tileData[index >> 1] = (m_tileData[index >> 1] & 0xF0) | (data);

	if (m_pLevel->m_pDimension->field_E)
	{
		m_pLevel->updateLight(LightLayer::Block, globalX, y, globalZ, globalX, y, globalZ);
		lightGaps(x, z);
	}

	if (Tile::lightBlock[tile])
	{
		if (height <= y)
			recalcHeight(x, y + 1, z);
	}
	else if (height - 1 == y)
	{
		recalcHeight(x, y, z);
	}

	m_pLevel->updateLight(LightLayer::Sky, globalX, y, globalZ, globalX, y, globalZ);
	m_pLevel->updateLight(LightLayer::Block, globalX, y, globalZ, globalX, y, globalZ);

	lightGaps(x, z);
	if (tile)
	{
		if (!m_pLevel->field_11)
			Tile::tiles[tile]->onPlace(m_pLevel, globalX, y, globalZ);
	}

	m_bUnsaved = true;
	m_updateMap[MakeHeightMapIndex(x, z)] |= 1 << (y >> 4);

	return true;
}

int LevelChunk::getData(int x, int y, int z)
{
	CheckPosition(x, y, z);

	int index = MakeBlockDataIndex(x, y, z);

	uint8_t data = m_tileData[index >> 1];
	if (index & 1)
		return data >> 4;
	return data & 0xF;
}

void LevelChunk::setData(int x, int y, int z, int data)
{
	CheckPosition(x, y, z);

	assert((data & ~0xF) == 0);
	data &= 0xF;

	int index = MakeBlockDataIndex(x, y, z);

	uint8_t& xdata = m_tileData[index >> 1];
	if (index & 1)
		xdata = (xdata & 0x0F) | (data << 4);
	else
		xdata = (xdata & 0xF0) | (data);
}

// seems to set block data in 8192 block (4*16*128) chunks for some reason ?
void LevelChunk::setBlocks(uint8_t* pData, int y)
{
	puts("LevelChunk::setBlocks");
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
				recalcHeight(x3, 0, j);
			}
		}
	}

	int globalX = 16 * m_chunkX + x1;
	int globalZ = 16 * m_chunkZ;

	m_pLevel->updateLight(LightLayer::Sky,   globalX, 0, globalZ, globalX + 4, 128, globalZ + 16);
	m_pLevel->updateLight(LightLayer::Block, globalX, 0, globalZ, globalX + 4, 128, globalZ + 16);
	m_pLevel->setTilesDirty(globalX, 0, globalZ, globalX + 4, 128, globalZ); //@BUG: not setting all the way to globalZ+16?
}

int LevelChunk::setBlocksAndData(uint8_t* pData, int a3, int a4, int a5, int a6, int a7, int a8, int a9)
{
	puts("LevelChunk::setBlocksAndData");

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
			uint8_t* dst = &m_pBlockData[MakeBlockDataIndex(x1, a4, x3)];
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
			uint8_t* dst = &m_tileData[MakeBlockDataIndex(x1, a4, x3) >> 1];
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
			uint8_t* dst = &m_lightBlk[MakeBlockDataIndex(x1, a4, x3) >> 1];
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
			uint8_t* dst = &m_lightSky[MakeBlockDataIndex(x1, a4, x3) >> 1];
			memcpy(dst, src, x5);
			src += x5;
			a9 += x5;
		}
	}

	return a9;
}

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
			uint8_t* src = &m_pBlockData[MakeBlockDataIndex(x1, a4, x3)];
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
			uint8_t* src = &m_tileData[MakeBlockDataIndex(x1, a4, x3) >> 1];
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
			uint8_t* src = &m_lightBlk[MakeBlockDataIndex(x1, a4, x3) >> 1];
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
			uint8_t* src = &m_lightSky[MakeBlockDataIndex(x1, a4, x3) >> 1];
			memcpy(dst, src, x5);
			dst += x5;
			a9 += x5;
		}
	}

	return a9;
}

Random LevelChunk::getRandom(TLong l)
{
	Random random;

	int levelSeed = m_pLevel->getSeed();
	int chunkSeed = m_chunkX * (4987142 * m_chunkX + 5947611) + m_chunkZ * (4392871 * m_chunkZ + 389711);

	random.init_genrand((levelSeed + chunkSeed) ^ l);

	return random;
}

bool LevelChunk::isEmpty()
{
	return false;
}

// EmptyLevelChunk

EmptyLevelChunk::EmptyLevelChunk(Level* pLevel, TileID* pBlockData, int x, int z)
	:LevelChunk(pLevel, pBlockData, x, z)
{
}

int EmptyLevelChunk::getHeightmap(int x, int z)
{
	return 0;
}

void EmptyLevelChunk::recalcHeightmap()
{

}

void EmptyLevelChunk::recalcHeightmapOnly()
{

}

int EmptyLevelChunk::getRawBrightness(int x, int y, int z, int skySubtract)
{
	return 7;
}

void EmptyLevelChunk::addEntity(Entity*)
{

}

void EmptyLevelChunk::removeEntity(Entity*)
{

}

void EmptyLevelChunk::removeEntity(Entity*, int vec)
{

}

bool EmptyLevelChunk::isSkyLit(int x, int y, int z)
{
	return false;
}

void EmptyLevelChunk::lightLava()
{

}

void EmptyLevelChunk::recalcBlockLights()
{

}

void EmptyLevelChunk::skyBrightnessChanged()
{

}

void EmptyLevelChunk::load()
{

}

void EmptyLevelChunk::unload()
{

}

void EmptyLevelChunk::markUnsaved()
{

}

TileID EmptyLevelChunk::getTile(int x, int y, int z)
{
	return Tile::invisible_bedrock->m_ID;
}

bool EmptyLevelChunk::setTile(int x, int y, int z, TileID tile)
{
	return true;
}

bool EmptyLevelChunk::setTileAndData(int x, int y, int z, TileID tile, int data)
{
	return true;
}

int EmptyLevelChunk::getData(int x, int y, int z)
{
	return 0;
}

void EmptyLevelChunk::setData(int x, int y, int z, int data)
{

}

void EmptyLevelChunk::recalcHeight(int, int, int)
{

}

bool EmptyLevelChunk::isEmpty()
{
	return true;
}

