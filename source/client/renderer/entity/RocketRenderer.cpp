/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "RocketRenderer.hpp"
#include "client/app/Minecraft.hpp"

RocketRenderer::RocketRenderer() :
	m_tile()
{
	m_shadowRadius = 0.5f;
}

void RocketRenderer::render(Entity* entity, const Vec3& pos, float rot, float a)
{
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);

	float brightness = entity->getBrightness(1.0f);

	bindTexture(C_ITEMS_NAME);
	m_renderer.renderTile(&m_tile, 0, brightness);

	glPopMatrix();
}

FakeRocketTile::FakeRocketTile() : Tile(0, 16*2+14, Material::plant)
{
}

int FakeRocketTile::getRenderShape() const
{
	return SHAPE_CROSS;
}
