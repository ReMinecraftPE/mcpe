/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once
#include <string>
#include <map>
#include "world/level/levelgen/chunk/LevelChunk.hpp"
#include "world/level/levelgen/chunk/ChunkRefCount.hpp"
#include "world/level/levelgen/chunk/GridArea.hpp"
#include "world/entity/MobSpawnerData.hpp"
#include "world/entity/EntityType.hpp"

class Level;
class LevelChunk;
class Dimension;
class Random;
class ChunkViewSource;

class ChunkSource
{
public:
	enum LoadMode
	{
		LOAD_NONE,
		LOAD_DEFERRED
	};

	typedef std::unordered_map<ChunkPos, ChunkRefCount> ChunkMap_t;

protected:
	int m_chunkSide;
	Level* m_level;
	Dimension* m_dimension;
	ChunkSource* m_parent;
	std::unique_ptr<ChunkSource> m_ownedParent;

public:
	ChunkSource(Level* level, Dimension* dimension, int chunkSide);
	ChunkSource(ChunkSource& parent);
	ChunkSource(std::unique_ptr<ChunkSource> parent);
	virtual ~ChunkSource();

public:
	int getChunkSide() const
	{
		return m_chunkSide;
	}
	Level& getLevel() const
	{
		return *m_level;
	}
	const Level& getLevelConst() const
	{
		return *m_level;
	}
	Dimension& getDimension() const
	{
		return *m_dimension;
	}
	const Dimension& getDimensionConst() const
	{
		return *m_dimension;
	}
	LevelChunk* getGeneratedChunk(const ChunkPos& pos);
	LevelChunk* getAvailableChunk(const ChunkPos& pos);
	LevelChunk* getAvailableChunkAt(const TilePos& tilePos);
	virtual LevelChunk* getExistingChunk(const ChunkPos& pos);
	virtual LevelChunk* requestChunk(const ChunkPos& pos, LoadMode loadMode);
	virtual bool releaseChunk(LevelChunk& chunk);
	virtual bool postProcess(ChunkViewSource& chunkViewSource);
	virtual void loadChunk(LevelChunk& chunk);
	LevelChunk* getOrLoadChunk(const ChunkPos& pos, LoadMode loadMode);
	virtual void postProcessMobsAt(TileSource* tileSource, int, int, Random& random);
	virtual const std::vector<MobSpawnerData>& getMobsAt(TileSource& tileSource, EntityType entityType, const TilePos& pos);
	virtual bool saveLiveChunk(LevelChunk& chunk);
	void discard(std::unique_ptr<LevelChunk>& chunk);
	void discard(LevelChunk& chunk);
	virtual void hintDiscardBatchBegin();
	virtual void hintDiscardBatchEnd();
	virtual void acquireDiscarded(std::unique_ptr<LevelChunk>&& chunk);
	virtual const ChunkMap_t& getStoredChunks();
	virtual void compact();
	virtual void waitDiscardFinished();
	virtual GridArea<LevelChunk*> getView(const TilePos& tileMin, const TilePos& tileMax, LoadMode loadMode, bool circular, const std::function<void(LevelChunk*&)>& addCallback, const std::function<void(LevelChunk*&)>& moveCallback);
	GridArea<LevelChunk*> getView(const TilePos& pos, int spread, LoadMode loadMode, bool circular, const std::function<void(LevelChunk*&)>& addCallback, const std::function<void(LevelChunk*&)>& moveCallback);

protected:
	float _getMinPlayerDistance(const LevelChunk& chunk);

	void _fireChunkLoaded(LevelChunk& chunk);

	void _startPostProcessing(LevelChunk& chunk);
	void _startPostProcessingArea(LevelChunk& chunk);

	bool _asyncChunkTask(LevelChunk* chunk);
	void _asyncChunkTaskCallback(LevelChunk* chunk);

	bool _asyncPostProcessingTask(LevelChunk* chunk, std::shared_ptr<ChunkViewSource> cvs);
	void _asyncPostProcessingCallback(LevelChunk* chunk, std::shared_ptr<ChunkViewSource> cvs);
};

