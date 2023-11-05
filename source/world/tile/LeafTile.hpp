/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "TransparentTile.hpp"

class LeafTile : public TransparentTile
{
public:
	LeafTile(int id);
	~LeafTile();

	int getColor(LevelSource*, int, int, int) override;
	int getTexture(int dir, int data) override;
	bool isSolidRender() override;
	void onRemove(Level*, int x, int y, int z) override;
	void stepOn(Level*, int x, int y, int z, Entity*) override;
	void tick(Level*, int x, int y, int z, Random*) override;

	void die(Level*, int, int, int);

	int* field_70;
	int field_74;
	bool m_bUseBiomeColors;
};
