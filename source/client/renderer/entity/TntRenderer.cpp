/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TntRenderer.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "renderer/ShaderConstants.hpp"
#include "renderer/MatrixStack.hpp"
#include "world/entity/PrimedTnt.hpp"

TntRenderer::Materials::Materials()
{
	MATERIAL_PTR(switchable, primed_tnt);
}

TntRenderer::TntRenderer()
{
	m_shadowRadius = 0.5f;
}

void TntRenderer::render(const Entity& entity, const Vec3& pos, float rot, float a)
{
	const PrimedTnt& tnt = (const PrimedTnt&)entity;

	MatrixStack::Ref matrix = MatrixStack::World.push();
	matrix->translate(pos);

	float m = 1.0f + float(tnt.m_fuseTimer) - a;
	if (m < 10.0f)
	{
		float n = (m / -10.0f) + 1.0f;
		if (n < 0.0f)
			n = 0.0f;
		if (n > 1.0f)
			n = 1.0f;

		float scale = 1.0f + 0.3f * n * n * n * n;
		matrix->scale(scale);
	}

	bindTexture(C_TERRAIN_NAME);

	// @NOTE: Useless assignment. Already being done by the renderTile function
	Tesselator::instance.color(1.0f, 1.0f, 1.0f);

	// Render the base
#ifdef ENH_SHADE_HELD_TILES
#define ARGPATCH , 1.0f
#else
#define ARGPATCH
#endif
	
	m_tileRenderer.renderTile(FullTile(Tile::tnt, 0), m_shaderMaterials.entity ARGPATCH);

	// @NOTE: Converting to a uint8 for whatever reason
	if (((uint8_t(tnt.m_fuseTimer) / 5) & 1) == 0)
	{
		currentShaderColor = Color(1.0f, 1.0f, 1.0f, (((float(tnt.m_fuseTimer) - a) + 1.0f) / -100.0f + 1.0f) * 0.8f);
		m_tileRenderer.renderTile(FullTile(Tile::tnt, 0), m_shaderMaterials.entity ARGPATCH);
	}

#ifdef ARGPATCH
#undef ARGPATCH
#endif
}
