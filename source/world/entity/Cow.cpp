/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "Cow.hpp"
#include "common/Utils.hpp"

Cow::Cow(Level* pLevel) : Animal(pLevel)
{
	field_C8 = RENDER_COW;
	m_texture = "mob/cow.png";
	setSize(0.9f, 1.3f);
}