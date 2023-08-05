/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TripodCameraRenderer.hpp"
#include "Minecraft.hpp"

TripodCameraRenderer::TripodCameraRenderer() :
	m_tile(0, 243, Material::plant),
	m_cube(0, 0)
{
	m_cube.addBox(-4.0f, -4.0f, -6.0f, 8, 8, 10);
	m_cube.m_posY = 11.0f;
	field_4 = 0.5f;
}

float TripodCameraRenderer::getFlashTime(TripodCamera* camera, float f)
{
	if (camera->field_B90 > 7 || camera->field_B90 < 0)
		return -1.0f;

	return 0.125f * (float(camera->field_B90) - f);
}

void TripodCameraRenderer::render(Entity* entity, float x, float y, float z, float a, float b)
{
	glPushMatrix();
	glTranslatef(x, y, z);
	m_cube.field_C  = 0.017453f * (180.0f + 0.5f * entity->m_pitch);
	m_cube.field_10 = -0.017453f * entity->m_yaw;

	Tesselator& t = Tesselator::instance;
	t.color(1.0f, 1.0f, 1.0f);

	bindTexture("gui/items.png");
	t.begin();
	m_renderer.tesselateCrossTexture(&m_tile, 0, -0.5f, -0.5f, -0.5f);
	t.draw();

	bindTexture("item/camera.png");
	m_cube.render(0.0625f);

	Entity* pHREntity = m_pDispatcher->m_pMinecraft->m_hitResult.m_pEnt;

	float time = getFlashTime((TripodCamera*)entity, b);
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
		m_cube.renderHorrible(0.0625f);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
	}

	glPopMatrix();
}
