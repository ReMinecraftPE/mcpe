/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "Pig.hpp"
#include "common/Utils.hpp"

Pig::Pig(Level* pLevel) : Animal(pLevel)
{
	field_C8 = RENDER_PIG;
	m_texture = "mob/pig.png";
	setSize(0.9f, 0.9f);
	// some dataitem stuff
}

std::string Pig::getAmbientSound()
{
	return "mob.pig";
}

std::string Pig::getDeathSound()
{
	return "mob.pigdeath";
}

std::string Pig::getHurtSound()
{
	return "mob.pig";
}

int Pig::getEntityTypeId()
{
	return TYPE_PIG;
}

int Pig::getMaxHealth()
{
	return 10;
}

bool Pig::interact(Player* pPlayer)
{
	return false;
}

int Pig::getDeathLoot()
{
	if (isOnFire())
		return Item::porkChop_cooked->m_itemID;
	else
		return Item::porkChop_raw->m_itemID;
}

bool Pig::hasSaddle()
{
	return false;
}

void Pig::setSaddle(bool b)
{
	// @TODO: this
}

Entity* Pig::getBreedOffspring(Animal* pOther)
{
    return new Pig(m_pLevel);
}
