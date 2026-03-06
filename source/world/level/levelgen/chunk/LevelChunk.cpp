/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "common/Logger.hpp"
#include "world/level/Level.hpp"
#include "world/phys/AABB.hpp"
#include "world/level/TileSource.hpp"
#include "world/level/levelgen/biome/Biome.hpp"

LevelChunk::LevelChunk(Level& level, Dimension& dimension, const ChunkPos& pos, bool readOnly)
	: m_level(level),
	  m_dimension(dimension),
	  m_posMin(pos),
	  m_posMax(TilePos(pos) + TilePos(ChunkConstants::XZ_SIZE, ChunkConstants::Y_SIZE, ChunkConstants::XZ_SIZE) - 1),
	  m_pos(pos),
	  m_readOnly(readOnly),
	  m_biomeDirtyTicks(INT32_MIN),
	  m_terrainDirtyTicks(INT32_MIN),
	  m_tileEntityDirtyTicks(INT32_MIN),
	  m_entityDirtyTicks(INT32_MIN)
{
	memset(m_tiles, TILE_AIR, sizeof(m_tiles));
	memset(m_heightMap, 0, sizeof(m_heightMap));
	memset(m_unk85876, 0, sizeof(m_unk85876));
}

LevelChunk::~LevelChunk()
{
}

uint8_t LevelChunk::getHeightmap(const ChunkTilePos& pos) const
{
	AssertPos(pos);

	return m_heightMap[pos.index2D()];
}

void LevelChunk::recalcHeightmap()
{
	for (int x = 0; x < ChunkConstants::XZ_SIZE; x++)
	{
		for (int z = 0; x < ChunkConstants::XZ_SIZE; z++)
		{
			ChunkTilePos chunkTilePos(x, 0, z);

			m_unk85876[chunkTilePos.index2D()] = -999;

			// find the top most y-pos where there is a light blocking tile underneath
			{
				uint8_t y;
				for (y = ChunkConstants::Y_SIZE - 1; y > 0; y--)
				{
					TileID tile = getTile(ChunkTilePos(x, static_cast<int>(y - 1), z));
					if (Tile::lightBlock[tile] != 0)
						break;
				}

				m_heightMap[chunkTilePos.index2D()] = y;
			}

			if (!m_dimension.hasCeiling())
			{
				Brightness_t brightness = Brightness::MAX;
				for (int y = ChunkConstants::Y_SIZE - 1; y > 0; y--)
				{
					ChunkTilePos pos(x, y, z);
					TileID tile = getTile(pos);
					brightness -= Tile::lightBlock[tile];
					if (brightness <= 0)
						break;

					m_lightSky.set(pos, brightness);
				}
			}
		}
	}
}

int LevelChunk::getBrightness(const LightLayer& lightLayer, const ChunkTilePos& pos) const
{
	AssertPos(pos);

	if (lightLayer == LightLayer::Sky)
		return m_lightSky.get(pos);
	else if (lightLayer == LightLayer::Block)
		return m_lightBlock.get(pos);

	return Brightness::MIN;
}

void LevelChunk::setBrightness(const LightLayer& lightLayer, const ChunkTilePos& pos, Brightness_t brightness)
{
	AssertPos(pos);

	if (lightLayer == LightLayer::Sky)
		m_lightSky.set(pos, brightness);
	else if (lightLayer == LightLayer::Block)
		m_lightBlock.set(pos, brightness);
}

Brightness_t LevelChunk::getRawBrightness(const ChunkTilePos& pos, Brightness_t skyDampen) const
{
	Brightness_t skyBrightness = static_cast<Brightness_t>(m_lightSky.get(pos));
	if (skyBrightness > skyDampen)
		skyBrightness -= skyDampen;
	else
		skyBrightness = Brightness::MIN;

	Brightness_t blockBrightness = static_cast<Brightness_t>(m_lightBlock.get(pos));
	return skyBrightness > blockBrightness ? skyBrightness : blockBrightness;
}

bool LevelChunk::isSkyLit(const ChunkTilePos& pos) const
{
	AssertPos(pos);

	return m_heightMap[pos.index2D()] <= pos.y;
}

void LevelChunk::recalcBlockLights()
{
}

void LevelChunk::addEntity(std::unique_ptr<Entity> entity)
{
	entity->m_chunkPos = m_pos;
	m_entities.push_back(std::move(entity));
	_resetDirtyCounter(m_entityDirtyTicks, 0);
}

bool LevelChunk::removeEntity(Entity& entity)
{
	std::vector<std::unique_ptr<Entity>>::iterator iter = m_entities.begin();
	std::vector<std::unique_ptr<Entity>>::iterator end = m_entities.end();
	for (; iter != end; iter++)
	{
		if (iter->get() == &entity)
			break;
	}

	if (iter == end)
		return false;

	iter->release();

	std::iter_swap(iter, end - 1);
	m_entities.pop_back();

	_resetDirtyCounter(m_entityDirtyTicks, 0);
	return true;
}

void LevelChunk::moveEntity(std::unique_ptr<Entity>& entity)
{
	m_entities.push_back(std::move(entity));
	_resetDirtyCounter(m_entityDirtyTicks, 0);
}

void LevelChunk::getEntities(Entity* blacklistedEntity, const AABB& aabb, std::vector<Entity*>& output) const
{
	for (std::vector<std::unique_ptr<Entity>>::const_iterator iter = m_entities.begin(); iter != m_entities.end(); iter++)
	{
		Entity* entity = iter->get();
		if (entity == blacklistedEntity)
			continue;

		if (entity->m_hitbox.intersect(aabb))
			output.push_back(entity);
	}
}

void LevelChunk::getEntities(const EntityType& type, const AABB& aabb, std::vector<Entity*>& output) const
{
	for (std::vector<std::unique_ptr<Entity>>::const_iterator iter = m_entities.begin(); iter != m_entities.end(); iter++)
	{
		Entity* entity = iter->get();
		if (entity->getDescriptor().isType(type))
			continue;

		if (entity->m_hitbox.intersect(aabb))
			output.push_back(entity);
	}
}

FullTile LevelChunk::setTileAndData(const ChunkTilePos& pos, FullTile tile, TileSource* issuingSource)
{
	FullTile oldTile = getTileAndData(pos);
	if (oldTile != tile)
	{
		if (pos.y >= (m_unk85876[pos.index2D()] - 1))
			m_unk85876[pos.index2D()] = -999;

		bool tileChanged = oldTile.getType() != tile.getType();

		if (tileChanged)
		{
			_removeCallbacks(pos, oldTile.getTypeId(), tile.getTypeId(), issuingSource);
			m_tiles[pos.index()] = tile.getTypeId();
		}

		m_tileData.set(pos, tile.data);

		if (tileChanged)
		{
			_placeCallbacks(pos, oldTile.getTypeId(), tile.getTypeId(), issuingSource);
		}

		_resetDirtyCounter(m_terrainDirtyTicks, 0);
	}

	return oldTile;
}

bool LevelChunk::setData(const ChunkTilePos& pos, TileData data)
{
	if (m_tileData.set(pos, data))
	{
		_resetDirtyCounter(m_terrainDirtyTicks, 0);
		return true;
	}

	return false;
}

Biome* LevelChunk::getBiome(const ChunkTilePos& pos)
{
	//return Biome::getBiome(m_biomes[pos.index2D()].m_biomeId); // todo: add method to getBiome
	throw std::bad_cast();
}

uint32_t LevelChunk::getGrassColor(const ChunkTilePos& pos)
{
	BiomeChunkData& data = m_biomes[pos.index2D()];
	return (data.m_grassRed << 16) | (data.m_grassGreen << 8) | data.m_grassBlue;
}

void LevelChunk::deferLightEmitter(const TilePos& pos)
{
	std::lock_guard<std::mutex> lock(m_deferredLightMutex);
	m_deferredLightEmitterPositions.push_back(pos);
}

void LevelChunk::tick(Player* player, Tick_t tick)
{
	if (m_readOnly)
		return;

	if (m_finalization <= CF_NEEDS_POPULATION)
	{
		// TODO
		m_finalization = CF_DONE;
	}
	else if (false)
	{
		// TODO: handle m_pendingEntitiesSerialized
	}
	else if (player->getLevel().m_bIsClientSide)
	{

	}
	else
	{
		tickBlocks(player);
		// TODO
	}

	m_currentTick = tick;
}

static int RandomTickPositionValue = 42184323;

void LevelChunk::tickBlocks(Player* player)
{
	Level& level = player->getLevel();
	TileSource& source = player->getTileSource();

	for (int i = 0; i < 20; i++)
	{
		RandomTickPositionValue = 3 * RandomTickPositionValue + 1013904223;
		ChunkTilePos pos(
			(RandomTickPositionValue >> 2) & (ChunkConstants::XZ_SIZE - 1),
			(RandomTickPositionValue >> 18) & (ChunkConstants::Y_SIZE - 1),
			(RandomTickPositionValue >> 10) & (ChunkConstants::XZ_SIZE - 1)
		);

		TileID tile = getTile(pos);
		if (Tile::shouldTick[tile])
		{
			TilePos absolutePos = pos + m_posMin;
			Tile::tiles[tile]->tick(&source, absolutePos, &level.m_random);
		}
	}
}

void LevelChunk::_placeCallbacks(const ChunkTilePos& pos, TileID oldTileID, TileID newTileID, TileSource* issuingSource)
{
	TilePos absolutePos = pos + m_posMin;

	// TODO

	// TODO: tileentity stuff
}

void LevelChunk::_removeCallbacks(const ChunkTilePos&, TileID, TileID, TileSource*)
{
	// TODO: tileentity stuff
}

void LevelChunk::_lightGap(TileSource& source, const TilePos& pos)
{
	uint8_t heightmap = source.getHeightmap(pos);

	TilePos min(pos.x, std::min(pos.y, static_cast<int>(heightmap)), pos.z);
	TilePos max(pos.x, std::max(pos.y, static_cast<int>(heightmap)), pos.z);

	source.runLightUpdates(LightLayer::Sky, min, max);
}

void LevelChunk::_lightGaps(TileSource& source, const ChunkTilePos& pos)
{
	TilePos absolutePos = pos + m_posMin;
	AssertPos(absolutePos);

	absolutePos.y = getHeightmap(pos);

	_lightGap(source, absolutePos - TilePos(1, 0, 0));
	_lightGap(source, absolutePos + TilePos(1, 0, 0));
	_lightGap(source, absolutePos - TilePos(0, 0, 1));
	_lightGap(source, absolutePos + TilePos(0, 0, 1));
}

LevelChunk::NibbleTileArray& LevelChunk::getLight(const LightLayer& lightLayer)
{
	if (lightLayer == LightLayer::Sky)
		return m_lightSky;
	else
		return m_lightBlock;
}
