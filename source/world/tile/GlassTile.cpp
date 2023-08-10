/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "GlassTile.hpp"
#include "world/level/Level.hpp"

GlassTile::GlassTile(int a, int b, Material* c) : HalfTransparentTile(a, b, c)
{
}

int GlassTile::getResourceCount(Random* pRandom)
{
	return 0;
}