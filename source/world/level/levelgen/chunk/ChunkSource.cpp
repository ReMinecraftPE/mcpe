/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "world/level/levelgen/chunk/ChunkSource.hpp"
#include "world/level/levelgen/chunk/ChunkViewSource.hpp"
#include "world/level/levelgen/biome/Biome.hpp"
#include "world/level/Level.hpp"
#include "common/threading/BackgroundQueuePool.hpp"

ChunkSource::ChunkSource(Level* level, Dimension* dimension, int chunkSide)
	: m_chunkSide(chunkSide),
	  m_level(level),
	  m_dimension(dimension),
	  m_parent(nullptr),
	  m_ownedParent()
{
}

ChunkSource::ChunkSource(ChunkSource& parent)
	: m_chunkSide(parent.m_chunkSide),
	  m_level(parent.m_level),
	  m_dimension(parent.m_dimension),
	  m_parent(&parent),
	  m_ownedParent()
{
}

ChunkSource::ChunkSource(std::unique_ptr<ChunkSource> parent)
	: m_chunkSide(parent->m_chunkSide),
	  m_level(parent->m_level),
	  m_dimension(parent->m_dimension),
	  m_parent(parent.get()),
	  m_ownedParent(std::move(parent))
{
}

ChunkSource::~ChunkSource()
{
}

LevelChunk* ChunkSource::getGeneratedChunk(const ChunkPos& pos)
{
	LevelChunk* chunk = getExistingChunk(pos);
	if (!chunk)
		return nullptr;

	if (chunk->getState() <= CS_GENERATING)
		return nullptr;

	return chunk;
}

LevelChunk* ChunkSource::getAvailableChunk(const ChunkPos& pos)
{
	LevelChunk* chunk = getExistingChunk(pos);
	if (!chunk)
		return nullptr;

	if (chunk->getState() <= CS_POST_PROCESSING)
		return nullptr;

	return chunk;
}

LevelChunk* ChunkSource::getAvailableChunkAt(const TilePos& tilePos)
{
	return getAvailableChunk(tilePos);
}

LevelChunk* ChunkSource::getExistingChunk(const ChunkPos& pos)
{
	return nullptr;
}

LevelChunk* ChunkSource::requestChunk(const ChunkPos& pos, LoadMode loadMode)
{
	if (m_parent)
		return m_parent->requestChunk(pos, loadMode);

	return nullptr;
}

bool ChunkSource::releaseChunk(LevelChunk& chunk)
{
	if (m_parent)
		return m_parent->releaseChunk(chunk);

	return false;
}

bool ChunkSource::postProcess(ChunkViewSource& chunkViewSource)
{
	return true;
}

void ChunkSource::loadChunk(LevelChunk& chunk)
{
	if (m_parent)
		return m_parent->loadChunk(chunk);
}

bool ChunkSource::_asyncChunkTask(LevelChunk* chunk)
{
	if (chunk->tryChangeState(CS_UNLOADED, CS_GENERATING))
		loadChunk(*chunk);

	return true;
}

void ChunkSource::_asyncChunkTaskCallback(LevelChunk* chunk)
{
	if (!releaseChunk(*chunk))
		_startPostProcessingArea(*chunk);
}

LevelChunk* ChunkSource::getOrLoadChunk(const ChunkPos& pos, LoadMode loadMode)
{
	LevelChunk* chunk = requestChunk(pos, loadMode);

	if (!m_level || !m_level->m_bIsClientSide)
	{
		if (chunk && loadMode == LOAD_DEFERRED && chunk->getState() == CS_UNLOADED)
		{
			requestChunk(pos, LOAD_NONE);

			BackgroundQueuePool::GetFor(BackgroundQueuePool::QR_STREAMING).queue(
				std::bind(&ChunkSource::_asyncChunkTask, this, chunk),
				std::bind(&ChunkSource::_asyncChunkTaskCallback, this, chunk),
				static_cast<int>(_getMinPlayerDistance(*chunk)) + 16
			);
		}
	}

	return chunk;
}

void ChunkSource::postProcessMobsAt(TileSource* tileSource, int unk1, int unk2, Random& random)
{
	if (m_parent)
		m_parent->postProcessMobsAt(tileSource, unk1, unk2, random);
}

const std::vector<MobSpawnerData>& ChunkSource::getMobsAt(TileSource& tileSource, EntityType entityType, const TilePos& pos)
{
	if (m_parent)
		return m_parent->getMobsAt(tileSource, entityType, pos);

	return Biome::EMPTY_MOBLIST;
}

bool ChunkSource::saveLiveChunk(LevelChunk& chunk)
{
	if (m_parent)
		return m_parent->saveLiveChunk(chunk);

	return false;
}

void ChunkSource::discard(std::unique_ptr<LevelChunk>& chunk)
{
	if (m_parent)
		m_parent->acquireDiscarded(std::move(chunk));
}

void ChunkSource::discard(LevelChunk& chunk)
{
	discard(std::unique_ptr<LevelChunk>(&chunk));
	delete& chunk; // brother, what the fuck
}

void ChunkSource::hintDiscardBatchBegin()
{
	if (m_parent)
		return m_parent->hintDiscardBatchBegin();
}

void ChunkSource::hintDiscardBatchEnd()
{
	if (m_parent)
		return m_parent->hintDiscardBatchEnd();
}

void ChunkSource::acquireDiscarded(std::unique_ptr<LevelChunk>&& chunk)
{
	if (m_parent)
		return m_parent->acquireDiscarded(std::move(chunk));
}

const ChunkSource::ChunkMap_t& ChunkSource::getStoredChunks()
{
	assert(m_parent != nullptr);
	return m_parent->getStoredChunks();
}

void ChunkSource::compact()
{
	if (m_parent)
		return m_parent->compact();
}

void ChunkSource::waitDiscardFinished()
{
	if (m_parent)
		return m_parent->waitDiscardFinished();
}

LevelChunk* _chunkViewGenerate(ChunkSource::LoadMode loadMode, ChunkSource* source, const Vec3Int32& pos)
{
	ChunkPos chunkPos(pos.x, pos.y, pos.z);
	return source->getOrLoadChunk(chunkPos, loadMode);
}

void _chunkViewClear(ChunkSource::LoadMode loadMode, ChunkSource* source, LevelChunk*& chunk)
{
	source->releaseChunk(*chunk);
}

GridArea<LevelChunk*> ChunkSource::getView(const TilePos& tileMin, const TilePos& tileMax, LoadMode loadMode, bool circular, const std::function<void(LevelChunk*&)>& addCallback, const std::function<void(LevelChunk*&)>& moveCallback)
{
	TilePos minPos(tileMin.x, 0, tileMin.z);
	TilePos maxPos(tileMax.x, 0, tileMax.z);

	std::function<LevelChunk* (const Vec3Int32&)> generate = std::bind(&_chunkViewGenerate, loadMode, this, std::placeholders::_1);
	std::function<void(LevelChunk*&)> clearCallback = std::bind(&_chunkViewClear, loadMode, this, std::placeholders::_1);

	return GridArea<LevelChunk*>(minPos, maxPos, 16, generate, clearCallback, circular, addCallback, moveCallback);
}

GridArea<LevelChunk*> ChunkSource::getView(const TilePos& pos, int spread, LoadMode loadMode, bool circular, const std::function<void(LevelChunk*&)>& addCallback, const std::function<void(LevelChunk*&)>& moveCallback)
{
	return getView(pos - spread, pos + spread, loadMode, circular, addCallback, moveCallback);
}

void ChunkSource::_fireChunkLoaded(LevelChunk& chunk)
{
	if (m_level)
	{
		if (!m_level->m_bIsClientSide)
			m_level->onChunkLoaded(chunk);
	}
}

bool ChunkSource::_asyncPostProcessingTask(LevelChunk* chunk, std::shared_ptr<ChunkViewSource> cvs)
{
	if (!chunk->tryChangeState(CS_GENERATED, CS_POST_PROCESSING))
		return true;

	bool success = chunk->getGenerator()->postProcess(*cvs);
	if (success)
	{
		chunk->changeState(CS_POST_PROCESSING, CS_POST_PROCESSED);
		return true;
	}
	else
	{
		chunk->changeState(CS_POST_PROCESSING, CS_GENERATED);
		return false;
	}
}

void ChunkSource::_asyncPostProcessingCallback(LevelChunk* chunk, std::shared_ptr<ChunkViewSource> cvs)
{
	if (chunk->getState() == CS_POST_PROCESSED)
		_fireChunkLoaded(*chunk);

	cvs->clear();
}

void ChunkSource::_startPostProcessing(LevelChunk& chunk)
{
	std::shared_ptr<ChunkViewSource> cvs(new ChunkViewSource(*this, LOAD_NONE));

	cvs->move(chunk.getMinPos() - ChunkConstants::XZ_SIZE, chunk.getMaxPos() + ChunkConstants::XZ_SIZE);

	BackgroundQueuePool::GetFor(BackgroundQueuePool::QR_STREAMING).queue(
		std::bind(&ChunkSource::_asyncPostProcessingTask, this, &chunk, cvs),
		std::bind(&ChunkSource::_asyncPostProcessingCallback, this, &chunk, cvs),
		static_cast<int>(_getMinPlayerDistance(chunk)) + 32
	);
}
