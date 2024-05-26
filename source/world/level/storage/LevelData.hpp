/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include "BitStream.h"
#include "common/Utils.hpp"
#include "world/phys/Vec3.hpp"
#include "world/item/Inventory.hpp"

struct PlayerData
{
	Vec3 m_pos;
	Vec3 m_vel;
	float m_pitch;
	float m_yaw;
	float m_distanceFallen;
	short field_24;
	short field_26;
	bool field_28;
	int m_hotbar[C_MAX_HOTBAR_ITEMS];

	void loadPlayer(Player* player);
	void savePlayer(Player* player);
};

struct LevelData
{
private:
	int32_t m_seed;
	Pos m_spawnPos;
	int32_t m_time;
	int m_lastPlayed;
	int32_t m_sizeOnDisk;
	int field_1C;
	int m_storageVersion;
	int m_generatorVersion;
	std::string m_levelName;

private:
	void _init(int32_t seed = 0, int x = 0);
	void _init(int32_t seed, int storageVersion, const std::string& name);

public:
	// @TODO: Make private when level v2 is done
	PlayerData m_LocalPlayerData;
	int m_nPlayers;

	LevelData() { _init(); }
	LevelData(int32_t seed, const std::string& name, int storageVersion) { _init(seed, storageVersion, name); }


	void read(RakNet::BitStream& bs, int d);
	void write(RakNet::BitStream& bs);

	/* Getters & Setters */

	int32_t getSeed() const { return m_seed; }
	int getXSpawn() const { return m_spawnPos.x; }
	int getYSpawn() const { return m_spawnPos.y; }
	int getZSpawn() const { return m_spawnPos.z; }
	int32_t getTime() const { return m_time; }
	int32_t getSizeOnDisk() const { return m_sizeOnDisk; }
	//CompoundTag getLoadedPlayerTag(); // Return type may actually be a pointer, not sure

	void setSeed(int32_t seed) { m_seed = seed; }
	void setXSpawn(int xSpawn) { m_spawnPos.x = xSpawn; }
	void setYSpawn(int ySpawn) { m_spawnPos.y = ySpawn; }
	void setZSpawn(int zSpawn) { m_spawnPos.z = zSpawn; }
	void setTime(int32_t time) { m_time = time; }
	void setSizeOnDisk(int32_t sizeOnDisk) { m_sizeOnDisk = sizeOnDisk; }

	void setSpawn(int x, int y, int z)
	{
		m_spawnPos.x = x;
		m_spawnPos.y = y;
		m_spawnPos.z = z;
	}

	int getGeneratorVersion() const { return m_generatorVersion; }
	void setGeneratorVersion(int generatorVersion) { m_generatorVersion = generatorVersion; }

	int getLastPlayed() const { return m_lastPlayed; }

	// inlined in 0.1.0 demo
	int getStorageVersion() const { return m_storageVersion; }
	void setStorageVersion(int storageVersion) { m_storageVersion = storageVersion; }

	GameType getGameType() const
	{
#ifdef TEST_SURVIVAL_MODE
		return GAME_TYPE_SURVIVAL;
#else
		return GAME_TYPE_CREATIVE;
#endif
	}
	void setGameType(GameType gameType) { /* Empty in 0.2.1 */ }

	// @TODO: Not Implemented
	bool getSpawnMobs() { return false; }
	void setSpawnMobs(bool spawnMobs) { }

	std::string getLevelName() const { return m_levelName; }
	void setLevelName(const std::string& name) { m_levelName = name; }

	void setLoadedPlayerTo(Player* player);
};

