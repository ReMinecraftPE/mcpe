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

	int getRenderLayer() override;
	int getResourceCount(Random*) override;
	void onRemove(Level*, int x, int y, int z) override;
	bool shouldRenderFace(LevelSource*, int x, int y, int z, int dir) override;
	void tick(Level*, int x, int y, int z, Random*) override;
};
