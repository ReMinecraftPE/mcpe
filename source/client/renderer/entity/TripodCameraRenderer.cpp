/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TripodCameraRenderer.hpp"
#include "client/app/Minecraft.hpp"
#include "renderer/ShaderConstants.hpp"

TripodCameraRenderer::TripodCameraRenderer() :
	m_tile(),
	m_modelPart(0, 0)
{
	m_modelPart.addBox(-4.0f, -4.0f, -6.0f, 8, 8, 10);
	m_modelPart.m_pos.y = 11.0f;
	m_modelPart.m_pMaterial = &m_shaderMaterials.entity;
	m_shadowRadius = 0.5f;
}

float TripodCameraRenderer::getFlashTime(const TripodCamera& camera, float f)
{
	if (camera.field_B90 > 7 || camera.field_B90 < 0)
		return -1.0f;

	return 0.125f * (float(camera.field_B90) - f);
}

void TripodCameraRenderer::render(const Entity& entity, const Vec3& pos, float rot, float a)
{
	MatrixStack::Ref matrix = MatrixStack::World.push();
	matrix->translate(pos);

	m_modelPart.m_rot.x  = 0.017453f * (180.0f + 0.5f * entity.m_rot.y);
	m_modelPart.m_rot.y = -0.017453f * entity.m_rot.x;

	//Tesselator& t = Tesselator::instance;
	//t.color(1.0f, 1.0f, 1.0f);

	float brightness = entity.getBrightness(1.0f);

	bindTexture(C_ITEMS_NAME);
	//t.begin();
	//m_renderer.tesselateCrossTexture(&m_tile, 0, -0.5f, -0.5f, -0.5f);
	m_renderer.renderTile(FullTile(&m_tile, 0), m_shaderMaterials.entity_alphatest, brightness, false);
	//t.draw();

	bindTexture("item/camera.png");
	m_modelPart.setBrightness(brightness);
	m_modelPart.render(0.0625f);

	Entity* pHREntity = m_pDispatcher->m_pMinecraft->m_hitResult.m_pEnt;

	const TripodCamera& camera = (const TripodCamera&)entity;

	float time = getFlashTime(camera, a);
	if (time >= 0.0f)
	{
		currentShaderColor = Color::WHITE;
		currentShaderDarkColor = Color(1.0f, 1.0f, 1.0f, sinf(float(M_PI) * 2.0f * time));
	}

	if (&entity == pHREntity)
	{
#ifndef FEATURE_GFX_SHADERS
		//glDisable(GL_TEXTURE_2D);
#endif
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // default
		// @TODO FIX: With ENH_ENTITY_SHADING on, the cube is fully opaque.
		currentShaderColor = Color(0.5f, 0.5f, 0.5f, 0.5f);
		m_modelPart.render(0.0625f, &m_shaderMaterials.entity_color_overlay);
		//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		//glDisable(GL_BLEND);
#ifndef FEATURE_GFX_SHADERS
		//glEnable(GL_TEXTURE_2D);
#endif
	}
}

TripodTile::TripodTile() : Tile(0, 243, Material::plant)
{
}

int TripodTile::getRenderShape() const
{
	return SHAPE_CROSS;
}
