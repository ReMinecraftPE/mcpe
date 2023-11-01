/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "CreativeMode.hpp"
#include "client/app/Minecraft.hpp"

CreativeMode::CreativeMode(Minecraft* pMC) : GameMode(pMC)
{
}

bool CreativeMode::destroyBlock(int x, int y, int z, int i)
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

#ifndef ENH_INSTA_BREAK

// @NOTE: Duplicate of SurvivalMode's break logic!

void CreativeMode::startDestroyBlock(int x, int y, int z, int i)
{
	TileID tile = m_pMinecraft->m_pLevel->getTile(x, y, z);

	if (tile <= 0)
		return;

	if (m_destroyProgress == 0.0f)
	{
		Tile::tiles[tile]->attack(m_pMinecraft->m_pLevel, x, y, z, m_pMinecraft->m_pLocalPlayer);
	}

	if (Tile::tiles[tile]->getDestroyProgress(m_pMinecraft->m_pLocalPlayer) >= 1.0f)
	{
		destroyBlock(x, y, z, i);
	}

	return;
}

void CreativeMode::continueDestroyBlock(int x, int y, int z, int i)
{
	if (m_destroyCooldown > 0)
	{
		m_destroyCooldown--;
		return;
	}

	if (m_destroyingX != x || m_destroyingY != y || m_destroyingZ != z)
	{
		m_destroyProgress = 0.0f;
		m_lastDestroyProgress = 0.0f;
		m_destroyTicks = 0;
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
	m_destroyProgress += 16.0f * destroyProgress;
	m_destroyTicks++;

	if ((m_destroyTicks & 3) == 1)
	{
		m_pMinecraft->m_pSoundEngine->play("step." + pTile->m_pSound->m_name,
			float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f,
			0.5f * (1.0f + pTile->m_pSound->field_18), 0.8f * pTile->m_pSound->field_1C);
	}

	if (m_destroyProgress >= 1.0f)
	{
		destroyBlock(m_destroyingX, m_destroyingY, m_destroyingZ, i);
		m_destroyTicks = 0;
		m_destroyCooldown = 5;
		m_destroyProgress = 0.0f;
		m_lastDestroyProgress = 0.0f;
	}
}

void CreativeMode::stopDestroyBlock()
{
	m_destroyProgress = 0.0f;
	m_destroyCooldown = 0;
}

void CreativeMode::tick()
{
	m_lastDestroyProgress = m_destroyProgress;
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

#endif

void CreativeMode::initPlayer(Player* p)
{
	p->m_yaw = -180.0f;
	p->m_pInventory->prepareCreativeInventory();
}

float CreativeMode::getPickRange()
{
	return 5.0f;
}

bool CreativeMode::isCreativeType()
{
	return true;
}

bool CreativeMode::isSurvivalType()
{
	return false;
}
