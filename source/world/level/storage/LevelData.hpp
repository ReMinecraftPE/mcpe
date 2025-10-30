/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include "BitStream.h"
#include "world/phys/Vec3.hpp"
#include "world/item/Inventory.hpp"

#define LEVEL_STORAGE_VERSION_DEFAULT 2

struct PlayerData
{
	Vec3 m_pos;
	Vec3 m_vel;
	Vec2 m_rot;
	float m_distanceFallen;
	short field_24;
	short field_26;
	bool field_28;
	int m_hotbar[C_MAX_HOTBAR_ITEMS];

	void loadPlayer(Player& player) const;
	void savePlayer(const Player& player);
};

struct LevelSettings
{
	int32_t m_seed;
	GameType m_gameType;

	LevelSettings(int32_t seed = 0, GameType gameType = GAME_TYPE_CREATIVE)
	{
		m_seed = seed;
		m_gameType = gameType;
	}
};

struct LevelData
{
private:
	void _init();
	void _init(const LevelSettings& settings, int storageVersion = 0);
	void _init(const LevelSettings& settings, int storageVersion, const std::string& name);

	void _setLastPlayed(int lastPlayed) { m_lastPlayed = lastPlayed; }

public:
	// @TODO: Make private when level v2 is done
	PlayerData m_LocalPlayerData;
	int m_nPlayers;

	LevelData() { _init(); }
	LevelData(const LevelSettings& settings, const std::string& name, int storageVersion) { _init(settings, storageVersion, name); }
	~LevelData();

	void v1_read(RakNet::BitStream& bs, int storageVersion);
	void v1_write(RakNet::BitStream& bs) const;
	void read(RakNet::BitStream& bs, int storageVersion);
	void write(RakNet::BitStream& bs, const std::vector<Player*>* players) const;

	CompoundTag createTag() const;
	CompoundTag createTag(const std::vector<Player*>& players) const;

	void loadTagData(CompoundTag& tag);
	void writeTagData(CompoundTag& levelTag, CompoundTag* playerTag) const;

	/* Getters & Setters */

	int32_t getSeed() const { return m_seed; }
	void setSeed(int32_t seed) { m_seed = seed; }

	int getXSpawn() const { return m_spawnPos.x; }
	void setXSpawn(int xSpawn) { m_spawnPos.x = xSpawn; }
	int getYSpawn() const { return m_spawnPos.y; }
	void setYSpawn(int ySpawn) { m_spawnPos.y = ySpawn; }
	int getZSpawn() const { return m_spawnPos.z; }
	void setZSpawn(int zSpawn) { m_spawnPos.z = zSpawn; }

	const TilePos& getSpawn() const { return m_spawnPos; }
	void setSpawn(const TilePos& pos) { m_spawnPos = pos; }

	int32_t getTime() const { return m_time; }
	void setTime(int32_t time) { m_time = time; }

	int32_t getSizeOnDisk() const { return m_sizeOnDisk; }
	void setSizeOnDisk(int32_t sizeOnDisk) { m_sizeOnDisk = sizeOnDisk; }

	const CompoundTag* getLoadedPlayerTag() const { return m_playerTag; }
	void setLoadedPlayerTag(CompoundTag* playerTag);
	void setPlayerTag(const CompoundTag* playerTag);

	int getDimension() const { return m_dimensionId; }
	void setDimension(int dimensionId) { m_dimensionId = dimensionId; }

	int getGeneratorVersion() const { return m_generatorVersion; }
	void setGeneratorVersion(int generatorVersion) { m_generatorVersion = generatorVersion; }

	int32_t getLastPlayed() const { return m_lastPlayed; }

	// inlined in 0.1.0 demo
	int getStorageVersion() const { return m_storageVersion; }
	void setStorageVersion(int storageVersion) { m_storageVersion = storageVersion; }

	GameType getGameType() const;
	void setGameType(GameType gameType) { m_gameType = gameType; } // Empty and uncalled in 0.2.1

	bool getSpawnMobs() const { return m_bSpawnMobs; }
	void setSpawnMobs(bool spawnMobs) { m_bSpawnMobs = spawnMobs; }

	std::string getLevelName() const { return m_levelName; }
	void setLevelName(const std::string& name) { m_levelName = name; }

	void setLoadedPlayerTo(Player& player) const;

private:
	std::string m_levelName;
	int32_t m_seed;
	TilePos m_spawnPos;
	int32_t m_time;
	int32_t m_lastPlayed;
	int32_t m_sizeOnDisk;
	CompoundTag* m_playerTag;
	int m_dimensionId;
	GameType m_gameType;
	int m_storageVersion;
	bool m_bSpawnMobs;
	int m_generatorVersion;
};

