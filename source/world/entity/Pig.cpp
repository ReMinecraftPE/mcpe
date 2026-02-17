/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "Pig.hpp"
#include "Player.hpp"
#include "world/level/Level.hpp"

Pig::Pig(Level* pLevel) : Animal(pLevel)
{
	m_pDescriptor = &EntityTypeDescriptor::pig;
	m_renderType = RENDER_PIG;
	m_texture = "mob/pig.png";
	setSize(0.9f, 0.9f);
	// some dataitem stuff
	setSaddle(true);
}
int Pig::getDeathLoot() const
{
	return (isOnFire()) ? 
		Item::porkChop_cooked->m_itemID :
		Item::porkChop_raw->m_itemID;
}

bool Pig::interact(Player* pPlayer)
{
	return false;
	// @TODO: add saddles
	/*
    if (m_pLevel->m_bIsClientSide)
	{
		return false;
	}

	if (!m_bSaddled)
	{
		return false;
	}
	
	Entity* rider = getRider();

	// already being ridden by someone else
	if (rider && rider != pPlayer)
	{
		return false;
	}

	pPlayer->ride(this);
	return true;
	*/
}

void Pig::setSaddle(bool b)
{
	// @TODO: this
}
