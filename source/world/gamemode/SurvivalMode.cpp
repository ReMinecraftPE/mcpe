/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SurvivalMode.hpp"
#include "GameMods.hpp"
#include "client/app/Minecraft.hpp"
#include "network/packets/RemoveBlockPacket.hpp"

SurvivalMode::SurvivalMode(Minecraft* pMC, Level& level) : GameMode(pMC, level),
	m_destroyingPos(-1, -1, -1),
	m_destroyProgress(0.0f),
	m_lastDestroyProgress(0.0f),
	m_destroyTicks(0),
	m_destroyCooldown(0)
{
}

void SurvivalMode::initPlayer(Player* p)
{
	p->m_rot.x = -180.0f;
	p->m_pInventory->prepareSurvivalInventory();
}

bool SurvivalMode::canHurtPlayer()
{
	return true;
}

bool SurvivalMode::startDestroyBlock(Player* player, const TilePos& pos, Facing::Name face)
{
	ItemInstance* item = player->getSelectedItem();
	if (item && item->getItem() == Item::bow)
		return true;

	TileID tile = _level.getTile(pos);

	if (tile <= 0)
		return false;

	if (m_destroyProgress == 0.0f)
	{
		Tile::tiles[tile]->attack(&_level, pos, player);
	}

	if (Tile::tiles[tile]->getDestroyProgress(player) >= 1.0f)
	{
		return destroyBlock(player, pos, face);
	}

	return false;
}

bool SurvivalMode::destroyBlock(Player* player, const TilePos& pos, Facing::Name face)
{
	TileID tile = _level.getTile(pos);
	int    data = _level.getData(pos);

	bool changed = GameMode::destroyBlock(player, pos, face);

	bool couldDestroy = player->canDestroy(Tile::tiles[tile]);
	ItemInstance* item = player->getSelectedItem();
	if (item)
	{
		item->mineBlock(pos, face);
		if (item->m_count == 0)
		{
			item->snap(player);
			player->removeSelectedItem();
		}
	}

	if (changed && couldDestroy)
	{
#ifdef MOD_POCKET_SURVIVAL
		ItemInstance tileItem(tile, 1, data);
		if (tile == TILE_GRASS || !player->m_pInventory->hasUnlimitedResource(&tileItem))
		{
			Tile::tiles[tile]->playerDestroy(&_level, player, pos, data);
		}
#else
		Tile::tiles[tile]->playerDestroy(&_level, player, pos, data);
#endif
	}

	return changed;
}

bool SurvivalMode::continueDestroyBlock(Player* player, const TilePos& pos, Facing::Name face)
{
	if (m_destroyCooldown > 0)
	{
		m_destroyCooldown--;
		return false;
	}

	if (m_destroyingPos != pos)
	{
		m_destroyProgress     = 0.0f;
		m_lastDestroyProgress = 0.0f;
		m_destroyTicks = 0;
		m_destroyingPos = pos;
		return false;
	}

	TileID tile = _level.getTile(m_destroyingPos);
	if (!tile)
		return false;

	Tile* pTile = Tile::tiles[tile];
	float destroyProgress = pTile->getDestroyProgress(m_pMinecraft->m_pLocalPlayer);
	m_destroyProgress += getDestroyModifier() * destroyProgress;
	m_destroyTicks++;

	if ((m_destroyTicks & 3) == 1)
	{
		_level.playSound(pos + 0.5f, "step." + pTile->m_pSound->m_name,
			0.125f * (1.0f + pTile->m_pSound->volume), 0.5f * pTile->m_pSound->pitch);
	}

	if (m_destroyProgress >= 1.0f)
	{
		m_destroyTicks    = 0;
		m_destroyCooldown = 5;
		m_destroyProgress     = 0.0f;
		m_lastDestroyProgress = 0.0f;
		return destroyBlock(player, m_destroyingPos, face);
	}

	return false;
}

void SurvivalMode::stopDestroyBlock()
{
	m_destroyProgress = 0.0f;
	m_destroyCooldown = 0;
}

void SurvivalMode::tick()
{
	m_lastDestroyProgress = m_destroyProgress;
	//m_pMinecraft->m_pSoundEngine->playMusicTick(); // also on MultiPlayerGameMode
	GameMode::tick();
}

void SurvivalMode::render(float f)
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

bool SurvivalMode::useItemOn(Player* player, Level* level, ItemInstance* instance, const TilePos& pos, Facing::Name face)
{
#ifdef MOD_POCKET_SURVIVAL
	if (!instance)
		return GameMode::useItemOn(player, level, instance, pos, face);

	int oldCount = instance->m_count;
	bool result = GameMode::useItemOn(player, level, instance, pos, face);
	if (player->m_pInventory->hasUnlimitedResource(instance))
		instance->m_count = oldCount;

	return result;
#else
	return GameMode::useItemOn(player, level, instance, pos, face);
#endif
}