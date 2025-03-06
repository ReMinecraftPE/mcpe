/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class ClayTile : public Tile
{
public:
	ClayTile(int ID, int texture, Material*);

	int getResource(int, Random*) const override;
	int getResourceCount(Random*) const override;
};
