/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "HalfTransparentTile.hpp"

class IceTile : public HalfTransparentTile
{
public:
	IceTile(int ID, int texture, Material*);

	int getResourceCount(Random*) const override;
	void onRemove(Level*, const TilePos& pos) override;
	bool shouldRenderFace(const LevelSource*, const TilePos& pos, Facing::Name face) const override;
	void tick(Level*, const TilePos& pos, Random*) override;
};
