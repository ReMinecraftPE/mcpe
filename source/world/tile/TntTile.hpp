/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class TntTile : public Tile
{
public:
	TntTile(int ID, int texture);

public:
	int getResourceCount(Random*) const override;
	int getTexture(Facing::Name face) const override;
	void neighborChanged(TileSource*, const TilePos& pos, TileID tile) override;
	void destroy(TileSource*, const TilePos& pos, TileData data) override;
	void wasExploded(TileSource*, const TilePos& pos) override;
};
