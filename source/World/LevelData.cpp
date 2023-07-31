/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LevelData.hpp"

LevelData::LevelData()
{
}

LevelData::LevelData(TLong seed, const std::string& name, int x)
{
	m_seed = seed;
	field_20 = x;
	field_78 = name;
}

void PlayerData::loadPlayer(Player* player)
{
	player->setPos(0.0f, 0.0f, 0.0f);

	player->m_pos    = m_pos;
	player->field_3C = m_pos;
	player->field_98 = m_pos;
	player->m_vel.x = Mth::abs(m_vel.x) > 10.0f ? 0.0f : m_vel.x;
	player->m_vel.y = Mth::abs(m_vel.y) > 10.0f ? 0.0f : m_vel.y;
	player->m_vel.z = Mth::abs(m_vel.z) > 10.0f ? 0.0f : m_vel.z;

	player->m_pitch = player->field_60 = m_pitch;
	player->m_yaw   = player->field_5C = m_yaw;
	player->m_distanceFallen = m_distanceFallen;
	player->field_C0 = field_24;
	player->field_BC = field_26;
	player->field_7C = field_28;

	// @NOTE: Why are we updating m_pos, field_3C and field_98 above if we do this?
	player->setPos(m_pos.x, m_pos.y, m_pos.z);

	for (int i = 0; i < C_MAX_HOTBAR_ITEMS; i++)
		player->m_pInventory->setSelectionSlotItemId(i, m_hotbar[i]);
}
