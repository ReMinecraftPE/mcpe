/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "GameMode.hpp"
#include "client/app/Minecraft.hpp"

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
	Tile* pTile = Tile::tiles[_level.getTile(pos)];
	if (!pTile)
		return false;

	m_pMinecraft->m_pParticleEngine->destroyEffect(pos);

	int tileData = _level.getData(pos);
	pTile->playerWillDestroy(player, pos, face);
	bool bChanged = _level.setTile(pos, 0);
	if (!bChanged)
		return false;


	_level.playSound(pos + 0.5f, "step." + pTile->m_pSound->m_name,
		(pTile->m_pSound->volume * 0.5f) + 0.5f, pTile->m_pSound->pitch * 0.8f);

	pTile->destroy(&_level, pos, tileData);

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
}

void GameMode::render(float f)
{
}

float GameMode::getPickRange() const
{
/*
  if ( *inputMode == 1 )
	return 5.7;
  if ( *inputMode == 3 )
	return 5.6;
  if ( !player || player->isCreative() )
	return 12.0;
  return 5.0;
*/
	return 7.5f;
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
	if (tile > 0 && Tile::tiles[tile]->use(level, pos, player))
		return true;

	if (instance)
		return instance->useOn(player, level, pos, face);

	return false;
}

