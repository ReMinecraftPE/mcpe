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
	SpongeTile(int ID, int texture);

	void onPlace(Level*, int x, int y, int z) override;
	void destroy(Level*, int x, int y, int z, int dir) override;
};