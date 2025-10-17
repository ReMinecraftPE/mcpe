/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "GameMode.hpp"
#include "client/app/Minecraft.hpp"
#include "network/packets/RemoveBlockPacket.hpp"

GameMode::GameMode(Minecraft* pMinecraft, Level& level) :
	m_pMinecraft(pMinecraft),
	_level(level),
	field_8(0)
{
}

GameMode::~GameMode()
{
}

void GameMode::initLevel(Level* pLevel)
{
}

bool GameMode::startDestroyBlock(Player* player, const TilePos& pos, Facing::Name face)
{
	//if (!player->getCarriedItem()) // && not a bow
	return destroyBlock(player, pos, face);
}

bool GameMode::destroyBlock(Player* player, const TilePos& pos, Facing::Name face)
{
	Tile* oldTile = Tile::tiles[_level.getTile(pos)];
	if (!oldTile)
		return false;

	m_pMinecraft->m_pParticleEngine->destroyEffect(pos);

	int tileData = _level.getData(pos);
	oldTile->playerWillDestroy(player, pos, face);
	bool changed = _level.setTile(pos, TILE_AIR);
	if (!changed)
		return false;


	_level.playSound(pos + 0.5f, "step." + oldTile->m_pSound->m_name,
		(oldTile->m_pSound->volume * 0.5f) + 0.5f, oldTile->m_pSound->pitch * 0.8f);

	oldTile->destroy(&_level, pos, tileData);

	if (m_pMinecraft->isOnline())
	{
		m_pMinecraft->m_pRakNetInstance->send(new RemoveBlockPacket(player->m_EntityID, pos));
	}

	return true;
}

bool GameMode::continueDestroyBlock(Player* player, const TilePos& pos, Facing::Name face)
{
	return false;
}

void GameMode::stopDestroyBlock()
{
}

void GameMode::tick()
{
	// @NOTE: should only be in SurvivalMode & MultiPlayerGameMode, but Minecraft music is awesome
	m_pMinecraft->m_pSoundEngine->playMusicTick();
}

void GameMode::render(float f)
{
}

float GameMode::getBlockReachDistance() const
{
	/* Logic from Pocket Edition 0.12.1
	if ( *inputMode == 1 )
		return 5.7f;
	if ( *inputMode == 3 )
		return 5.6f;
	if ( !player || player->isCreative() )
		return 12.0f;
	*/

	// Fallback on Java and Pocket. All GameModes on PE are 5.0f until 0.10.0-0.12.1
	return 5.0f;
}

float GameMode::getEntityReachDistance() const
{
	return 5.0f;
}

LocalPlayer* GameMode::createPlayer(Level* pLevel)
{
	return new LocalPlayer(m_pMinecraft, pLevel, m_pMinecraft->m_pUser, pLevel->getDefaultGameType(), _level.m_pDimension->field_50);
}

void GameMode::initPlayer(Player* pPlayer)
{
}

void GameMode::adjustPlayer(Player* pPlayer)
{
}

bool GameMode::canHurtPlayer()
{
	return false;
}

void GameMode::interact(Player* player, Entity* entity)
{
	player->interact(entity);
}

void GameMode::attack(Player* player, Entity* entity)
{
	player->attack(entity);
}

int GameMode::handleInventoryMouseClick(int a, int b, int c, Player* player)
{
	return 0;
}

void GameMode::handleCloseInventory(int a, Player* player)
{
}

bool GameMode::useItem(Player* player, Level* level, ItemInstance* instance)
{
	int oldCount = instance->m_count;

	if (instance == instance->use(level, player))
		return instance->m_count != oldCount;

	return true;
}

bool GameMode::useItemOn(Player* player, Level* level, ItemInstance* instance, const TilePos& pos, Facing::Name face)
{
	TileID tile = level->getTile(pos);
	if (tile == Tile::invisible_bedrock->m_ID)
		return false;

	bool success = false;

	if (tile > 0 && Tile::tiles[tile]->use(level, pos, player))
	{
		success = true;
	}
	else if (instance)
	{
		success = instance->useOn(player, level, pos, face);
	}

	if (success)
	{
		_level.m_pRakNetInstance->send(new UseItemPacket(pos, face, player->m_EntityID, instance));
	}

	return success;
}

