/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <cstring>
#include <vector>
#include <algorithm>
#include "AABB.hpp"
#include "Utils.hpp"
#include "LightLayer.hpp"

class Level;
class Entity;

class LevelChunk
{
public:
	LevelChunk(Level*, int x, int z);
	LevelChunk(Level*, TileID* pBlockData, int x, int z);
	virtual ~LevelChunk();

	void init();

	void lightGap(int gx, int gz, uint8_t heightMap);
	void lightGaps(int x, int z);
	void deleteBlockData();
	void clearUpdateMap();

	virtual bool isAt(int x, int z);
	virtual int getHeightmap(int x, int z);
	virtual void recalcHeightmap();
	virtual void recalcHeightmapOnly();
	virtual int getBrightness(const LightLayer& ll, int x, int y, int z);
	virtual void setBrightness(const LightLayer& ll, int x, int y, int z, int brightness);
	virtual int getRawBrightness(int x, int y, int z, int skySubtract);
	virtual void addEntity(Entity*);
	virtual void removeEntity(Entity*);
	virtual void removeEntity(Entity*, int vec);
	virtual bool isSkyLit(int x, int y, int z);
	virtual void lightLava();
	virtual void recalcBlockLights();
	virtual void skyBrightnessChanged();
	virtual void load();
	virtual void unload();
	virtual bool shouldSave(bool b);
	virtual void markUnsaved();
	virtual int  countEntities();
	virtual void getEntities(Entity* pEntExclude, const AABB&, std::vector<Entity*>& out);
	virtual TileID getTile(int x, int y, int z);
	virtual bool setTile(int x, int y, int z, TileID tile);
	virtual bool setTileAndData(int x, int y, int z, TileID tile, int data);
	virtual int  getData(int x, int y, int z);
	virtual void setData(int x, int y, int z, int data);
	virtual void setBlocks(uint8_t* pData, int y);
	virtual int  getBlocksAndData(uint8_t* pData, int, int, int, int, int, int, int);
	virtual int  setBlocksAndData(uint8_t* pData, int, int, int, int, int, int, int);
	virtual Random getRandom(TLong l);
	virtual void recalcHeight(int, int, int);
	virtual bool isEmpty();
	//...

public:
	static bool touchedSky;

public:

	int field_4 = 0;
	bool m_bLoaded = false;
	Level* m_pLevel;
	uint8_t* m_tileData = nullptr;
	int      m_tileDataCnt = 0;
	uint8_t* m_lightSky = nullptr;
	int      m_lightSkyCnt = 0;
	uint8_t* m_lightBlk = nullptr;
	int      m_lightBlkCnt = 0;
	uint8_t m_heightMap[256];
	uint8_t m_updateMap[256];
	int field_228;
	int m_chunkX = 0;
	int m_chunkZ = 0;
	uint8_t field_234 = 0;
	bool m_bUnsaved = false;
	uint8_t field_236 = 0;
	uint8_t field_237 = 0;
	int field_238 = 0;
	int field_23C = 0;
	TileID* m_pBlockData = nullptr;
	std::vector<Entity*> m_entities[128 / 16];
};

//@OVERSIGHT: Why the hell is EmptyLevelChunk derived from the WHOLE of LevelChunk?!
class EmptyLevelChunk : public LevelChunk
{
public:
	EmptyLevelChunk(Level*, TileID* pBlockData, int x, int z);

	int getHeightmap(int x, int z) override;
	void recalcHeightmap() override;
	void recalcHeightmapOnly() override;
	int getRawBrightness(int x, int y, int z, int skySubtract) override;
	void addEntity(Entity*) override;
	void removeEntity(Entity*) override;
	void removeEntity(Entity*, int vec) override;
	bool isSkyLit(int x, int y, int z) override;
	void lightLava() override;
	void recalcBlockLights() override;
	void skyBrightnessChanged() override;
	void load() override;
	void unload() override;
	void markUnsaved() override;
	TileID getTile(int x, int y, int z) override;
	bool setTile(int x, int y, int z, TileID tile) override;
	bool setTileAndData(int x, int y, int z, TileID tile, int data) override;
	int  getData(int x, int y, int z) override;
	void setData(int x, int y, int z, int data) override;
	void recalcHeight(int, int, int) override;
	bool isEmpty() override;
};
