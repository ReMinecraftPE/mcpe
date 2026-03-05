/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class GrassTile : public Tile
{
public:
	static const Color DEFAULT_COLOR;

public:
	GrassTile(TileID ID, Material*);

public:
	int getResource(TileData, Random*) const override;
	int getColor(TileSource*, const TilePos& pos) const override;
	int getTexture(Facing::Name face) const override;
	int getTexture(TileSource*, const TilePos& pos, Facing::Name face) const override;
	void tick(TileSource*, const TilePos& pos, Random*) override;
};
