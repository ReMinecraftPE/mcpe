/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ClothTile.hpp"
#include "world/level/Level.hpp"

ClothTile::ClothTile(int id, int type) : Tile(id, TEXTURE_CLOTH_64, Material::cloth)
{
	field_6C = type;

	m_TextureFrame = getTexture(Facing::DOWN, type);
}

int ClothTile::getTexture(Facing::Name face) const
{
	return getTexture(face, field_6C);
}

int ClothTile::getTexture(Facing::Name face, int data) const
{
	//@HUH: what?
	return ((~(this->field_6C & 0xFu) >> 3) & 1) + 16 * (~(this->field_6C & 0xF) & 7) + 113;
}

int ClothTile::getSpawnResourcesAuxValue(int val) const
{
	return val;
}
