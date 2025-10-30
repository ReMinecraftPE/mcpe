/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "Cow.hpp"

Cow::Cow(Level* pLevel) : Animal(pLevel)
{
	m_pDescriptor = &EntityTypeDescriptor::cow;
	field_C8 = RENDER_COW;
	m_texture = "mob/cow.png";
	setSize(0.9f, 1.3f);
}

void Cow::addAdditionalSaveData(CompoundTag& tag) const
{
	Animal::addAdditionalSaveData(tag);
}

void Cow::readAdditionalSaveData(const CompoundTag& tag)
{
	Animal::readAdditionalSaveData(tag);
}