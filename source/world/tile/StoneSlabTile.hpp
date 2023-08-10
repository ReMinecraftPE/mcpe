/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class StoneSlabTile : public Tile
{
public:
	StoneSlabTile(int ID, bool bFull);

	bool isSolidRender() override;
	bool isCubeShaped() override;
	int getResource(int, Random*) override;
	int getResourceCount(Random*) override;
	int getSpawnResourcesAuxValue(int) override;
	int getTexture(int dir) override;
	int getTexture(int dir, int data) override;
	void onPlace(Level*, int x, int y, int z) override;
	bool shouldRenderFace(LevelSource*, int x, int y, int z, int dir) override;

	bool m_bFull;
};
