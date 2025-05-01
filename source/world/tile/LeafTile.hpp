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

	int getColor(const LevelSource*, const TilePos& pos) const override;
	int getTexture(Facing::Name face, int data) const override;
	bool isSolidRender() const override;
	void onRemove(Level*, const TilePos& pos) override;
	void stepOn(Level*, const TilePos& pos, Entity*) override;
	void tick(Level*, const TilePos& pos, Random*) override;

	void die(Level*, const TilePos& pos);

	int* field_70;
	int field_74;
};
