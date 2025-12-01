/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "GlassTile.hpp"
#include "world/level/Level.hpp"
#include "client/renderer/PatchManager.hpp"

GlassTile::GlassTile(int a, int b, Material* c) : HalfTransparentTile(a, b, c)
{
}

int GlassTile::getResourceCount(Random* pRandom) const
{
	return 0;
}

Tile::RenderLayer GlassTile::getRenderLayer() const
{
	if (GetPatchManager()->IsGlassSemiTransparent())
		return RENDER_LAYER_ALPHATEST;

	return RENDER_LAYER_OPAQUE;
}
