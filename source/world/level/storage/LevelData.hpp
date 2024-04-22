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
	void _init(TLong seed = 0, int x = 0);
	void _init(TLong seed, int version, const std::string& name);

public:
	LevelData() { _init(); }
	LevelData(TLong seed, const std::string& name, int version) { _init(seed, version, name); }


	void read(RakNet::BitStream& bs, int d);
	void write(RakNet::BitStream& bs);

	TLong m_seed;
	Pos m_spawnPos;
	TLong m_time;
	int m_lastPlayed;
	TLong field_18;
	int field_1C;
	int m_version;
	PlayerData m_LocalPlayerData;
	int m_nPlayers;
	std::string m_levelName;

	// inlined in 0.1.0 demo
	int getVersion() const
	{
		return m_version;
	}

	GameType getGameType() const
	{
#ifdef TEST_SURVIVAL_MODE
		return GAME_TYPE_SURVIVAL;
#else
		return GAME_TYPE_CREATIVE;
#endif
	}

	void setLevelName(const std::string& name)
	{
		m_levelName = name;
	}
};

