/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "CreativeMode.hpp"
#include "client/app/Minecraft.hpp"

CreativeMode::CreativeMode(Minecraft* pMC, Level& level) : GameMode(pMC, level),
	m_destroyingPos(-1, -1, -1),
	m_destroyProgress(0.0f),
	m_lastDestroyProgress(0.0f),
	m_destroyTicks(0),
	m_destroyCooldown(0)
{
}

bool CreativeMode::destroyBlock(Player* player, const TilePos& pos, Facing::Name face)
{
	_level.extinguishFire(player, pos, face);
	return GameMode::destroyBlock(player, pos, face);
}

bool CreativeMode::startDestroyBlock(Player* player, const TilePos& pos, Facing::Name face)
{
	ItemInstance* item = player->getSelectedItem();
	if (item && item->getItem() == Item::bow)
		return true;

	m_destroyCooldown = 5;
	return destroyBlock(player, pos, face);
}

bool CreativeMode::continueDestroyBlock(Player* player, const TilePos& pos, Facing::Name face)
{
	if (m_destroyCooldown - 1 > 0)
	{
		m_destroyCooldown--;
		return false;
	}

	m_destroyCooldown = 5;
	return destroyBlock(player, pos, face);
}

void CreativeMode::stopDestroyBlock()
{
	m_destroyProgress = 0.0f;
	m_destroyCooldown = 0;
}

void CreativeMode::tick()
{
	m_lastDestroyProgress = m_destroyProgress;
	GameMode::tick();
}

void CreativeMode::render(float f)
{
	if (m_destroyProgress <= 0.0f)
	{
		m_pMinecraft->m_gui.field_8 = 0.0f;
		m_pMinecraft->m_pLevelRenderer->field_10 = 0.0f;
	}
	else
	{
		float x = m_lastDestroyProgress + (m_destroyProgress - m_lastDestroyProgress) * f;
		m_pMinecraft->m_gui.field_8 = x;
		m_pMinecraft->m_pLevelRenderer->field_10 = x;
	}
}

void CreativeMode::initPlayer(Player* p)
{
	p->m_rot.x = -180.0f;
	p->m_pInventory->prepareCreativeInventory();
}
