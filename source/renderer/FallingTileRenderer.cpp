/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Utils.hpp" // include the utils only to let us know what enhancements we have
#if defined(ENH_ALLOW_SAND_GRAVITY)
#include "FallingTileRenderer.hpp"
#include "FallingTile.hpp"

FallingTileRenderer::FallingTileRenderer()
{
	field_4 = 0.5f;
}

void FallingTileRenderer::render(Entity* entity, float x, float y, float z, float a6, float a7)
{
	FallingTile* fallingTile = (FallingTile*)entity;

	glPushMatrix();
	glTranslatef(x, y, z);

	bindTexture(C_TERRAIN_NAME);

	// @NOTE: Useless assignment. Already being done by the renderTile function
	Tesselator::instance.color(1.0f, 1.0f, 1.0f);

	// Render the base
#ifdef ENH_SHADE_HELD_TILES
#define ARGPATCH , 1.0f
#else
#define ARGPATCH
#endif
	
	m_tileRenderer.renderTile(Tile::tiles[fallingTile->m_id], 0 ARGPATCH);

	glPopMatrix();

#ifdef ARGPATCH
#undef ARGPATCH
#endif
}

#endif
