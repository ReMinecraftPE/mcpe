#pragma once

#include "world/level/Level.hpp"

class MultiPlayerLevel : public Level
{
private:
	struct ResetInfo
	{
		TilePos pos;
		int ticks;
		TileID tile;
		TileData data;

		ResetInfo(const TilePos& pos, TileID tile, TileData data)
		{
			this->pos = pos;
			this->ticks = 80;
			this->tile = tile;
			this->data = data;
		}
	};

public:
	MultiPlayerLevel(LevelStorage* pStor, const std::string& name, const LevelSettings& settings, int storageVersion = LEVEL_STORAGE_VERSION_DEFAULT, Dimension* pDimension = nullptr);

public:
	void tick() override;
	ChunkSource* createChunkSource() override;
	void putEntity(int id, Entity* e);

private:
	std::vector<ResetInfo> m_updatesToReset;
	std::map<int, Entity*> m_entitiesById;
	EntityVector m_forced;
	EntityVector m_reEntries;
};