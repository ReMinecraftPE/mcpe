/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "MobRenderer.hpp"
#include "EntityRenderDispatcher.hpp"

MobRenderer::MobRenderer(Model* pModel, float f)
{
	m_pModel = pModel;
	field_4 = f;
}

void MobRenderer::setArmor(Model* model)
{
	m_pArmorModel = model;
}

int MobRenderer::prepareArmor(Mob* mob, int a, float b)
{
	return 0;
}

void MobRenderer::additionalRendering(Mob* mob, float f)
{
}

float MobRenderer::getAttackAnim(Mob* mob, float f)
{
	return mob->getAttackAnim(f);
}

float MobRenderer::getBob(Mob* mob, float f)
{
	return float(mob->field_B4) + f;
}

float MobRenderer::getFlipDegrees(Mob* mob)
{
	return 90.0f;
}

int MobRenderer::getOverlayColor(Mob* mob, float a, float b)
{
	return 0;
}

void MobRenderer::scale(Mob*, float)
{

}

void MobRenderer::setupPosition(Entity* entity, float x, float y, float z)
{
	glTranslatef(x, y, z);
}

void MobRenderer::setupRotations(Entity* entity, float x, float y, float z)
{
	glRotatef(180.0f - y, 0.0f, 1.0f, 0.0f);
}

void MobRenderer::render(Entity* entity, float x, float y, float z, float unused, float f)
{
	Mob* pMob = (Mob*)entity;
	glPushMatrix();
	glDisable(GL_CULL_FACE);
	m_pModel->field_4 = getAttackAnim(pMob, f);
	m_pModel->field_8 = false;

	if (m_pArmorModel)
		m_pArmorModel->field_8 = m_pModel->field_8;

	float aYaw   = pMob->field_5C + (pMob->m_yaw   - pMob->field_5C) * f;
	float aPitch = pMob->field_60 + (pMob->m_pitch - pMob->field_60) * f;
	float fBob   = getBob(pMob, f);
	float fSmth  = pMob->field_EC + (pMob->field_E8 - pMob->field_EC) * f;

	setupPosition(pMob, x, y - pMob->field_84, z);
	setupRotations(pMob, fBob, fSmth, f);

	glScalef(-1.0f, -1.0f, 1.0f);
	scale(pMob, f);
	glTranslatef(0.0f, -1.5078f, 0.0f);

	float x1 = pMob->field_128 + (pMob->field_12C - pMob->field_128) * f;
	if (x1 > 1.0f)
		x1 = 1.0f;
	float x2 = pMob->field_130 - pMob->field_12C * (1.0f - f);

	bindTexture("mob/char.png");
	glEnable(GL_ALPHA_TEST);

	m_pModel->prepareMobModel(pMob, x2, x1, f);
	m_pModel->render(x2, x1, fBob, aYaw - fSmth, aPitch, 0.059375f);
	additionalRendering(pMob, f);
	
	float fBright = pMob->getBrightness(f);
	int iOverlayColor = getOverlayColor(pMob, fBright, f);

	if (GET_ALPHA(iOverlayColor) || pMob->field_104 > 0 || pMob->field_110 > 0)
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_EQUAL);

		if (pMob->field_104 > 0 || pMob->field_110 > 0)
		{
			glColor4f(fBright, 0.0f, 0.0f, 0.4f);
			m_pModel->render(x2, x1, fBob, aYaw - fSmth, aPitch, 0.0625f);
		}
		if (GET_ALPHA(iOverlayColor))
		{
			glColor4f(
				float(GET_RED(iOverlayColor)) / 255.0f,
				float(GET_GREEN(iOverlayColor)) / 255.0f,
				float(GET_BLUE(iOverlayColor)) / 255.0f,
				float(GET_ALPHA(iOverlayColor)) / 255.0f);

			m_pModel->render(x2, x1, fBob, aYaw - fSmth, aPitch, 0.0625f);
		}

		glDepthFunc(GL_LEQUAL);
		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_ALPHA_TEST);
	}
	
	glEnable(GL_CULL_FACE);
	glPopMatrix();
	renderName(pMob, x, y, z);
}

void MobRenderer::renderName(Mob*, float, float, float)
{

}

void MobRenderer::renderNameTag(Mob* mob, const std::string& str, float x, float y, float z, int a)
{
	if (mob->distanceToSqr(m_pDispatcher->m_pMob) > float(a * a))
		return;

	Font* font = getFont();

	glPushMatrix();
	glTranslatef(x + 0.0f, y + 2.3f, z);
	glNormal3f(0.0f, 1.0f, 0.0f);
	// billboard the name towards the camera
	glRotatef(-m_pDispatcher->m_yaw,   0.0f, 1.0f, 0.0f);
	glRotatef(-m_pDispatcher->m_pitch, 1.0f, 0.0f, 0.0f);
	glScalef(-0.026667f, -0.026667f, 0.026667f);
	glDepthMask(false);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);
	
	Tesselator& t = Tesselator::instance;
	t.begin();

	int width = font->width(str);
	float widthHalf = float(width / 2);

	t.color(0.0f, 0.0f, 0.0f, 0.25f);
	t.vertex(-1.0f - widthHalf, -1.0f, 0.0f);
	t.vertex(-1.0f - widthHalf, 8.0f, 0.0f);
	t.vertex(widthHalf + 1.0f, 8.0f, 0.0f);
	t.vertex(widthHalf + 1.0f, -1.0f, 0.0f);
	t.draw();

	glEnable(GL_TEXTURE_2D);

	font->draw(str, -font->width(str) / 2, 0, 0x20FFFFFF);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(true);

	font->draw(str, -font->width(str) / 2, 0, 0xFFFFFFFF);

	glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glPopMatrix();
}
