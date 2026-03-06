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
	MultiPlayerLevel(LevelStorage* pStor, const std::string& name, const LevelSettings& settings, int storageVersion = LEVEL_STORAGE_VERSION_DEFAULT);

public:
	void tick() override;
	void putEntity(int id, Entity* e);

private:
	std::vector<ResetInfo> m_updatesToReset;
	std::map<int, Entity*> m_entitiesById;
	Entity::Vector m_forced;
	Entity::Vector m_reEntries;
};
