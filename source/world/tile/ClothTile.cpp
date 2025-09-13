/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ClothTile.hpp"
#include "world/level/Level.hpp"

ClothTile::ClothTile(int id) : Tile(id, TEXTURE_CLOTH_64, Material::cloth)
{
}

int ClothTile::getTexture(Facing::Name face, int data) const
{
	if (!data) {
		return m_TextureFrame;
	}
	else {
		data = getColorFromData(data);
		return 113 + ((data & 8) >> 3) + (data & 7) * 16;
	}
}

int ClothTile::getSpawnResourcesAuxValue(int val) const
{
	return val;
}
