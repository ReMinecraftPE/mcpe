/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "Pig.hpp"

Pig::Pig(Level* pLevel) : Animal(pLevel)
{
	m_pDescriptor = &EntityTypeDescriptor::pig;
	m_renderType = RENDER_PIG;
	m_texture = "mob/pig.png";
	setSize(0.9f, 0.9f);
	// some dataitem stuff
}
int Pig::getDeathLoot() const
{
	if (isOnFire())
		return Item::cookedPorkchop->m_itemID;
	else
		return Item::rawPorkchop->m_itemID;
}

void Pig::setSaddle(bool b)
{
	// @TODO: this
}
