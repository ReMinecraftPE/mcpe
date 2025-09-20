/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Bush.hpp"

class Sapling : public Bush
{
public:
	Sapling(int id, int texture);

	int getTexture(Facing::Name face, int data) const override;
	void tick(Level*, const TilePos& pos, Random*) override;
	int getSpawnResourcesAuxValue(int x) const override;

	void growTree(Level*, const TilePos& pos, Random*);
};
