/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Tile.hpp"
#include "world/level/Level.hpp"

GravelTile::GravelTile(int a, int b, Material* c) : SandTile(a, b, c)
{
}

int GravelTile::getResource(int a, Random* b)
{
	return m_ID;
}
