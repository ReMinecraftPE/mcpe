/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

// @TODO: Abstract out into HeavyTile, then have GravelTile inherit
class SandTile : public Tile
{
public:
	SandTile(int ID, int texture, Material*);

	int getTickDelay() const override;
	void tick(Level* level, const TilePos& pos, Random* random) override;
	void neighborChanged(Level* level, const TilePos& pos, TileID tile) override;
	void onPlace(Level* level, const TilePos& pos) override;

	void checkSlide(Level* level, const TilePos& pos);
	static bool isFree(Level* level, const TilePos& pos);

	static bool instaFall;
};
