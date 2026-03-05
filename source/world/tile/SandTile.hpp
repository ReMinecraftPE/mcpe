/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class SandTile : public Tile
{
public:
	SandTile(int ID, int texture, Material*);

public:
	int getTickDelay() const override;
	void tick(TileSource* source, const TilePos& pos, Random* random) override;
	void neighborChanged(TileSource* source, const TilePos& pos, TileID tile) override;
	void onPlace(TileSource* source, const TilePos& pos) override;

	void checkSlide(TileSource* source, const TilePos& pos);

public:
	static bool isFree(TileSource* source, const TilePos& pos);

public:
	static bool instaFall;
};
