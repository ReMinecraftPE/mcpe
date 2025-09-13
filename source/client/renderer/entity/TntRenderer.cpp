/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TntRenderer.hpp"
#include "world/entity/PrimedTnt.hpp"

TntRenderer::TntRenderer()
{
	m_shadowRadius = 0.5f;
}

void TntRenderer::render(Entity* entity, const Vec3& pos, float rot, float a)
{
	PrimedTnt* tnt = (PrimedTnt*)entity;

	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);

	float m = 1.0f + float(tnt->m_fuseTimer) - a;
	if (m < 10.0f)
	{
		float n = (m / -10.0f) + 1.0f;
		if (n < 0.0f)
			n = 0.0f;
		if (n > 1.0f)
			n = 1.0f;

		float scale = 1.0f + 0.3f * n * n * n * n;
		glScalef(scale, scale, scale);
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
	
	m_tileRenderer.renderTile(Tile::tnt, 0 ARGPATCH);

	// @NOTE: Converting to a uint8 for whatever reason
	if (((uint8_t(tnt->m_fuseTimer) / 5) & 1) == 0)
	{
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
		glColor4f(1.0f, 1.0f, 1.0f, (((float(tnt->m_fuseTimer) - a) + 1.0f) / -100.0f + 1.0f) * 0.8f);
		m_tileRenderer.renderTile(Tile::tnt, 0 ARGPATCH);
		glColor4f(1.0f, 1.0, 1.0f, 1.0f);
		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
	}

	glPopMatrix();

#ifdef ARGPATCH
#undef ARGPATCH
#endif
}
