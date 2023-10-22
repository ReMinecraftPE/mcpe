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
