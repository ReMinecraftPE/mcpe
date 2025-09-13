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
	int getTexture(Facing::Name face) const override;
	int getTexture(Facing::Name face, int data) const override;
	void onPlace(Level*, const TilePos& pos) override;
	bool shouldRenderFace(const LevelSource*, const TilePos& pos, Facing::Name face) const override;

	bool m_bFull;

	static const char* types[4];
};
