/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "GameMode.hpp"
#include "Minecraft.hpp"

GameMode::GameMode(Minecraft* pMinecraft)
{
	m_pMinecraft = pMinecraft;
}

GameMode::~GameMode()
{
}

void GameMode::initLevel(Level* pLevel)
{
}

void GameMode::startDestroyBlock(int x, int y, int z, int i)
{
	destroyBlock(x, y, z, i);
}

bool GameMode::destroyBlock(int x, int y, int z, int i)
{
	Level* pLevel = m_pMinecraft->m_pLevel;
	Tile* pTile = Tile::tiles[pLevel->getTile(x, y, z)];
	int tileData = pLevel->getData(x, y, z);

	bool bChanged = pLevel->setTile(x, y, z, 0);

	if (pTile && bChanged)
	{
		m_pMinecraft->m_pSoundEngine->play("step." + pTile->m_pSound->m_name,
			float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f,
			0.5f * (1.0f + pTile->m_pSound->field_18), 0.8f * pTile->m_pSound->field_1C);

		pTile->destroy(pLevel, x, y, z, tileData);

		return true;
	}

	return false;
}

void GameMode::continueDestroyBlock(int x, int y, int z, int t)
{
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

float GameMode::getPickRange()
{
	return 7.5f;
}

LocalPlayer* GameMode::createPlayer(Level* pLevel)
{
	return new LocalPlayer(m_pMinecraft, pLevel, m_pMinecraft->m_pUser, pLevel->m_pDimension->field_50);
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

bool GameMode::isCreativeType()
{
	return true;
}

bool GameMode::isSurvivalType()
{
	return false;
}
