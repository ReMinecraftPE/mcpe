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

	int getResourceCount(Random*) const override;
	int getTexture(int dir) const override;
	void neighborChanged(Level*, int x, int y, int z, int d) override;
	void destroy(Level*, int x, int y, int z, int data) override;
	void wasExploded(Level*, int x, int y, int z) override;
};
