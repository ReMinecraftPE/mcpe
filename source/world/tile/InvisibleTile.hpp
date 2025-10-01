/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class InvisibleTile : public Tile
{
public:
	InvisibleTile(TileID ID, int texture, Material*);
	int getRenderShape() const override;
	bool mayPick() const override;
};
