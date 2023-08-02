/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>

#include "Utils.hpp"
#include "Vec3.hpp"
#include "Inventory.hpp"
#include "BitStream.h"

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
	LevelData();
	LevelData(TLong seed, const std::string&, int);

	void read(RakNet::BitStream& bs, int d);
	void write(RakNet::BitStream& bs);

	TLong m_seed = 0;
	Pos m_spawnPos;
	TLong field_10 = 0;
	int field_14 = 0;
	TLong field_18 = 0;
	int field_1C = 0;
	int field_20 = 0;
	PlayerData m_LocalPlayerData;
	int m_nPlayers = -1;
	std::string field_78;

	// inlined in 0.1.0 demo
	int getVersion() const
	{
		return field_20;
	}
};

