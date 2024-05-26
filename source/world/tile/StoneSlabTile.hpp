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

	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	int getResource(int, Random*) const override;
	int getResourceCount(Random*) const override;
	int getSpawnResourcesAuxValue(int) const override;
	int getTexture(int dir) const override;
	int getTexture(int dir, int data) const override;
	void onPlace(Level*, int x, int y, int z) override;
	bool shouldRenderFace(const LevelSource*, int x, int y, int z, int dir) const override;

	bool m_bFull;
};
