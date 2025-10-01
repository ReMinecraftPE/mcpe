/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class SpongeTile : public Tile
{
public:
	SpongeTile(TileID ID, int texture);

	void onPlace(Level*, const TilePos& pos) override;
	void destroy(Level*, const TilePos& pos, TileData data) override;
};