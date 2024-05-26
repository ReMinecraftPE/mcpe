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

	int getTickDelay() const override;
	void tick(Level* level, int x, int y, int z, Random* random) override;
	void neighborChanged(Level* level, int x, int y, int z, int dir) override;
	void onPlace(Level* level, int x, int y, int z) override;

	void checkSlide(Level* level, int x, int y, int z);
	static bool isFree(Level* level, int x, int y, int z);

	static bool instaFall;
};
