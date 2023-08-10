/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "InvisibleTile.hpp"
#include "world/level/Level.hpp"

InvisibleTile::InvisibleTile(int ID, int texture, Material* pMtl) :
	Tile(ID, texture, pMtl)
{

}

int InvisibleTile::getRenderShape()
{
	return SHAPE_NONE;
}

