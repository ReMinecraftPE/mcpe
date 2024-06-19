/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SandStoneTile.hpp"
#include "world/level/Level.hpp"

SandStoneTile::SandStoneTile(int a, int b, Material* c) : Tile(a, b, c)
{
}

int SandStoneTile::getTexture(Facing::Name face) const
{
	switch (face)
	{
	case Facing::DOWN:
		return TEXTURE_SANDSTONE_BOTTOM;
	case Facing::UP:
		return TEXTURE_SANDSTONE_TOP;
	}

	return m_TextureFrame;
}
