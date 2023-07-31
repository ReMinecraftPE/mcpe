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
};

struct LevelData
{
	LevelData();
	LevelData(TLong seed, const std::string&, int);

	TLong m_seed = 0;
	Pos m_spawnPos;
	TLong field_10 = 0;
	int field_14 = 0;
	int field_18 = 0;
	int field_1C = 0;
	int field_20 = 0;
	PlayerData m_LocalPlayerData;
	int m_nPlayers = -1;
	std::string field_78;
};

