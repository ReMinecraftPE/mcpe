/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class ClothTile : public Tile
{
public:
	ClothTile(int id, int type);
	int getTexture(Facing::Name face) const override;
	int getTexture(Facing::Name face, int data) const override;
	int getSpawnResourcesAuxValue(int val) const override;

	uint8_t field_6C;
};
