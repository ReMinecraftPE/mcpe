/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SurvivalMode.hpp"
#include "Minecraft.hpp"

SurvivalMode::SurvivalMode(Minecraft* pMC) : GameMode(pMC)
{

}

void SurvivalMode::initPlayer(Player* p)
{
	p->m_yaw = -180.0;
}

void SurvivalMode::startDestroyBlock(int x, int y, int z, int i)
{
	TileID tile = m_pMinecraft->m_pLevel->getTile(x, y, z);

	if (tile <= 0)
		return;

#ifdef ENH_INSTA_BREAK
	destroyBlock(x, y, z, i);
	return;
#endif

	if (field_18 == 0.0f)
	{
		Tile::tiles[tile]->attack(m_pMinecraft->m_pLevel, x, y, z, m_pMinecraft->m_pLocalPlayer);
	}

	if (Tile::tiles[tile]->getDestroyProgress(m_pMinecraft->m_pLocalPlayer) >= 1.0f)
	{
		destroyBlock(x, y, z, i);
	}
}

bool SurvivalMode::destroyBlock(int x, int y, int z, int i)
{
	m_pMinecraft->m_pParticleEngine->destroy(x, y, z);

	TileID tile = m_pMinecraft->m_pLevel->getTile(x, y, z);
	int    data = m_pMinecraft->m_pLevel->getData(x, y, z);

	if (!GameMode::destroyBlock(x, y, z, i))
		return false;

	//@HUH: check too late?
	bool bCanDestroy = m_pMinecraft->m_pLocalPlayer->canDestroy(Tile::tiles[tile]);

	if (bCanDestroy)
	{
		Tile::tiles[tile]->playerDestroy(m_pMinecraft->m_pLevel, m_pMinecraft->m_pLocalPlayer, x, y, z, data);
		
		if (m_pMinecraft->isOnline())
		{
			m_pMinecraft->m_pRakNetInstance->send(new RemoveBlockPacket(m_pMinecraft->m_pLocalPlayer->m_EntityID, x, y, z));
		}
	}

	return true;
}

void SurvivalMode::continueDestroyBlock(int x, int y, int z, int i)
{
	if (field_24 > 0)
	{
		field_24--;
		return;
	}

	if (m_destroyingX != x || m_destroyingY != y || m_destroyingZ != z)
	{
		field_18 = 0.0f;
		field_1C = 0.0f;
		field_20 = 0;
		m_destroyingX = x;
		m_destroyingY = y;
		m_destroyingZ = z;
		return;
	}

	TileID tile = m_pMinecraft->m_pLevel->getTile(m_destroyingX, m_destroyingY, m_destroyingZ);
	if (!tile)
		return;

	Tile* pTile = Tile::tiles[tile];
	float destroyProgress = pTile->getDestroyProgress(m_pMinecraft->m_pLocalPlayer);

	field_18 += 16.0f * destroyProgress;
	field_20++;

	if ((field_20 & 3) == 1)
	{
		m_pMinecraft->m_pSoundEngine->play("step." + pTile->m_pSound->m_name,
			float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f,
			0.5f * (1.0f + pTile->m_pSound->field_18), 0.8f * pTile->m_pSound->field_1C);
	}

	if (field_18 >= 1.0f)
	{
		destroyBlock(m_destroyingX, m_destroyingY, m_destroyingZ, i);
		field_20 = 0;
		field_24 = 5;
		field_18 = 0.0f;
		field_1C = 0.0f;
	}
}

void SurvivalMode::stopDestroyBlock()
{
	field_18 = 0.0f;
	field_24 = 0;
}

void SurvivalMode::tick()
{
	field_1C = field_18;
}

void SurvivalMode::render(float f)
{
	if (field_18 <= 0.0f)
	{
		m_pMinecraft->m_gui.field_8 = 0.0f;
		m_pMinecraft->m_pLevelRenderer->field_10 = 0.0f;
	}
	else
	{
		float x = field_1C + (field_18 - field_1C) * f;
		m_pMinecraft->m_gui.field_8 = x;
		m_pMinecraft->m_pLevelRenderer->field_10 = x;
	}
}

float SurvivalMode::getPickRange()
{
	return 5.0f;
}

bool SurvivalMode::isCreativeType()
{
	return false;
}

bool SurvivalMode::isSurvivalType()
{
	return true;
}

bool GameMode::useItem(Player* player, Level* level, ItemInstance* instance)
{
	int oldAmount = instance->m_amount;

	if (instance == instance->use(level, player))
		return instance->m_amount != oldAmount;

	return true;
}

bool GameMode::useItemOn(Player* player, Level* level, ItemInstance* instance, int x, int y, int z, int d)
{
	TileID tile = level->getTile(x, y, z);
	if (tile > 0 && Tile::tiles[tile]->use(level, x, y, z, player))
		return true;

	if (instance)
		return instance->useOn(player, level, x, y, z, d);

	return false;
}

