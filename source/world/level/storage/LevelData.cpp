/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LevelData.hpp"

#define FORCE_SURVIVAL_MODE (TEST_SURVIVAL_MODE || 0)

void LevelData::_init(int32_t seed, int storageVersion)
{
	m_seed = seed;
	m_time = 0;
	m_lastPlayed = 0;
	m_sizeOnDisk = 0;
	field_1C = 0;
	m_storageVersion = storageVersion;
	m_generatorVersion = 1; // pre-0.2.1 versions used storageVersion instead
	m_nPlayers = -1;
}

void LevelData::_init(int32_t seed, int storageVersion, const std::string& name)
{
	_init(seed, storageVersion);
	m_levelName = name;
}

void LevelData::read(RakNet::BitStream& bs, int storageVersion)
{
	m_storageVersion = storageVersion;
	bs.Read(m_seed);
	bs.Read(m_spawnPos);
	bs.Read(m_time);
	bs.Read(m_sizeOnDisk);
	bs.Read(m_lastPlayed);

	RakNet::RakString rs;
	bs.Read(rs);
	m_levelName = std::string(rs.C_String());
}

void LevelData::write(RakNet::BitStream& bs)
{
	bs.Write(m_seed);
	bs.Write(m_spawnPos);
	bs.Write(m_time);
	bs.Write(m_sizeOnDisk);
	bs.Write(int(getEpochTimeS()));

	RakNet::RakString rs(m_levelName.c_str());
	bs.Write(rs);
}

GameType LevelData::getGameType() const
{
#if FORCE_SURVIVAL_MODE
	return GAME_TYPE_SURVIVAL;
#else
	return GAME_TYPE_CREATIVE;
#endif
}

void LevelData::setLoadedPlayerTo(Player* player)
{
	if (m_nPlayers == 1)
		m_LocalPlayerData.loadPlayer(player);
}

void PlayerData::loadPlayer(Player* player)
{
	player->setPos(Vec3::ZERO);

	player->m_pos    = m_pos;
	player->m_oPos = m_pos;
	player->m_posPrev = m_pos;
	player->m_vel.x = Mth::abs(m_vel.x) > 10.0f ? 0.0f : m_vel.x;
	player->m_vel.y = Mth::abs(m_vel.y) > 10.0f ? 0.0f : m_vel.y;
	player->m_vel.z = Mth::abs(m_vel.z) > 10.0f ? 0.0f : m_vel.z;

	// Let the setter do the validation
	player->setRot(
		player->m_oRot = m_rot
	);
	player->m_distanceFallen = m_distanceFallen;
	player->m_fireTicks = field_24;
	player->m_airCapacity = field_26;
	player->m_onGround = field_28;

	// @NOTE: Why are we updating m_pos, m_oPos and m_posPrev above if we do this?
	player->setPos(m_pos);

	// TODO: survival mode stuff
	for (int i = 0; i < C_MAX_HOTBAR_ITEMS; i++)
		player->m_pInventory->setQuickSlotIndexByItemId(i, m_hotbar[i]);
}

void PlayerData::savePlayer(Player* player)
{
	m_pos = player->m_pos;
	m_vel = player->m_vel;
	m_rot = player->m_rot;
	m_distanceFallen = player->m_distanceFallen;
	field_24 = player->m_fireTicks;
	field_26 = player->m_airCapacity;
	field_28 = player->m_onGround;

	// TODO: survival mode stuff
	for (int i = 0; i < C_MAX_HOTBAR_ITEMS; i++)
		m_hotbar[i] = player->m_pInventory->getQuickSlotItemId(i);
}
