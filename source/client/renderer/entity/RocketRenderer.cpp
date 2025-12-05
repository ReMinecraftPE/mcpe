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

void RocketRenderer::render(const Entity& entity, const Vec3& pos, float rot, float a)
{
	MatrixStack::Ref matrix = MatrixStack::World.push();
	matrix->translate(pos);

	float brightness = entity.getBrightness(1.0f);

	bindTexture(C_ITEMS_NAME);
	m_renderer.renderTile(FullTile(&m_tile, 0), m_materials.entity_alphatest, brightness);
}

FakeRocketTile::FakeRocketTile() : Tile(0, 16*2+14, Material::plant)
{
}

eRenderShape FakeRocketTile::getRenderShape() const
{
	return SHAPE_CROSS;
}
