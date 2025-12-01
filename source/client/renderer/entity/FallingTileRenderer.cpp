/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "GameMods.hpp"
#ifdef ENH_ALLOW_SAND_GRAVITY
#include "FallingTileRenderer.hpp"
#include "renderer/MatrixStack.hpp"
#include "world/entity/FallingTile.hpp"

FallingTileRenderer::FallingTileRenderer()
{
	m_shadowRadius = 0.5f;
}

void FallingTileRenderer::render(Entity* entity, const Vec3& pos, float rot, float a)
{
	FallingTile* tile = (FallingTile*)entity;
	
#ifdef ENH_GFX_MATRIX_STACK
	MatrixStack::Ref matrix = MatrixStack::World.push();
	matrix->translate(pos);
#else
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
#endif

	bindTexture(C_TERRAIN_NAME);

	// @NOTE: Useless assignment. Already being done by the renderTile function
	Tesselator::instance.color(1.0f, 1.0f, 1.0f);

	// Render the base
#ifdef ENH_SHADE_HELD_TILES
#define ARGPATCH , entity->getBrightness(0.0f)
#else
#define ARGPATCH
#endif
	
	m_tileRenderer.renderTile(Tile::tiles[tile->m_id], 0 ARGPATCH);

#ifndef ENH_GFX_MATRIX_STACK
	glPopMatrix();
#endif

#ifdef ARGPATCH
#undef ARGPATCH
#endif
}

#endif
