/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TripodCameraRenderer.hpp"
#include "client/app/Minecraft.hpp"

TripodCameraRenderer::TripodCameraRenderer() :
	m_tile(),
	m_modelPart(0, 0)
{
	m_modelPart.addBox(-4.0f, -4.0f, -6.0f, 8, 8, 10);
	m_modelPart.m_pos.y = 11.0f;
	m_shadowRadius = 0.5f;
}

float TripodCameraRenderer::getFlashTime(TripodCamera* camera, float f)
{
	if (camera->field_B90 > 7 || camera->field_B90 < 0)
		return -1.0f;

	return 0.125f * (float(camera->field_B90) - f);
}

void TripodCameraRenderer::render(Entity* entity, const Vec3& pos, float rot, float a)
{
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	m_modelPart.m_rot.x  = 0.017453f * (180.0f + 0.5f * entity->m_rot.y);
	m_modelPart.m_rot.y = -0.017453f * entity->m_rot.x;

	//Tesselator& t = Tesselator::instance;
	//t.color(1.0f, 1.0f, 1.0f);

	float brightness = entity->getBrightness(1.0f);

	bindTexture(C_ITEMS_NAME);
	//t.begin();
	//m_renderer.tesselateCrossTexture(&m_tile, 0, -0.5f, -0.5f, -0.5f);
	m_renderer.renderTile(&m_tile, 0, brightness);
	//t.draw();

	bindTexture("item/camera.png");
	m_modelPart.setBrightness(brightness);
	m_modelPart.render(0.0625f);

	Entity* pHREntity = m_pDispatcher->m_pMinecraft->m_hitResult.m_pEnt;

	float time = getFlashTime((TripodCamera*)entity, a);
	if (time >= 0.0f)
	{
		glColor4f(1.0f, 1.0f, 1.0f, sinf(float(M_PI) * 2.0f * time));
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // @BUG: overwriting the sinf result with 1.0f
	}

	if (entity == pHREntity)
	{
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// @TODO FIX: With ENH_ENTITY_SHADING on, the cube is fully opaque.
		glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
		m_modelPart.render(0.0625f);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
	}

	glPopMatrix();
}

TripodTile::TripodTile() : Tile(0, 243, Material::plant)
{
}

int TripodTile::getRenderShape() const
{
	return SHAPE_CROSS;
}
