/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "MobRenderer.hpp"
#include "EntityRenderDispatcher.hpp"
#include "client/app/Minecraft.hpp"

MobRenderer::MobRenderer(Model* pModel, float f)
{
	m_pArmorModel = nullptr;
	m_pModel = pModel;
	m_shadowRadius = f;
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
	// @HACK: I eye-balled a corrective offset of 1/13, since I still can't figure out why all mobs are floating - Brent
	// This was due to "0.059375f" being used for scale instead of "0.0625f"
	glTranslatef(x, y, z);
}

void MobRenderer::setupRotations(Entity* entity, float x, float y, float z)
{
	glRotatef(180.0f - y, 0.0f, 1.0f, 0.0f);

	Mob* mob = (Mob*)entity;
	if (mob->field_110 > 0)
	{
		float t = Mth::sqrt((float(mob->field_110) + z - 1.0f) / 20.0f * 1.6f);
		if (t > 1.0f)
			t = 1.0f;

		glRotatef(getFlipDegrees(mob) * t, 0.0f, 0.0f, 1.0f);
	}
}

void MobRenderer::render(Entity* entity, float x, float y, float z, float unused, float f)
{
	Mob* pMob = (Mob*)entity;

	glPushMatrix();
	glDisable(GL_CULL_FACE);

	m_pModel->field_4 = getAttackAnim(pMob, f);
	m_pModel->m_bRiding = false;
	m_pModel->m_bIsBaby = pMob->isBaby();

	if (m_pArmorModel != nullptr)
	{
		m_pArmorModel->m_bRiding = m_pModel->m_bRiding;
		m_pArmorModel->m_bIsBaby = m_pModel->m_bIsBaby;
	}

	float aYaw   = pMob->m_rotPrev.x + (pMob->m_rot.x   - pMob->m_rotPrev.x) * f;
	float aPitch = pMob->m_rotPrev.y + (pMob->m_rot.y - pMob->m_rotPrev.y) * f;
	float fBob   = getBob(pMob, f);
	float fSmth  = pMob->field_EC + (pMob->field_E8 - pMob->field_EC) * f;

	setupPosition(pMob, x, y - pMob->m_heightOffset, z);
	setupRotations(pMob, fBob, fSmth, f);

	float fScale = 0.0625f; // the scale variable according to b1.2_02
	glScalef(-1.0f, -1.0f, 1.0f);
	scale(pMob, f);
	//glTranslatef(0.0f, -1.5078f, 0.0f);
	glTranslatef(0.0f, -24.0f * fScale - (1.0f / 128.0f), 0.0f);

	float x1 = pMob->field_128 + (pMob->field_12C - pMob->field_128) * f;
	if (x1 > 1.0f)
		x1 = 1.0f;
	float x2 = pMob->field_130 - pMob->field_12C * (1.0f - f);

	bindTexture(pMob->getTexture());
	glEnable(GL_ALPHA_TEST);

	m_pModel->setBrightness(entity->getBrightness(1.0f));
	m_pModel->prepareMobModel(pMob, x2, x1, f);
	m_pModel->render(x2, x1, fBob, aYaw - fSmth, aPitch, fScale); // last float here (scale) was set to "0.059375f" for some reason

	for (int i = 0; i < 4; i++)
	{
		if (prepareArmor(pMob, i, f))
		{
			m_pArmorModel->render(x2, x1, fBob, aYaw - fSmth, aPitch, fScale);
			glDisable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);
		}
	}

	additionalRendering(pMob, f);
	
	float fBright = pMob->getBrightness(f);
	int iOverlayColor = getOverlayColor(pMob, fBright, f);

	if (GET_ALPHA(iOverlayColor) || pMob->m_hurtTime > 0 || pMob->field_110 > 0)
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_EQUAL);

		if (pMob->m_hurtTime > 0 || pMob->field_110 > 0)
		{
			glColor4f(fBright, 0.0f, 0.0f, 0.4f);
			m_pModel->render(x2, x1, fBob, aYaw - fSmth, aPitch, fScale); // was 0.059375f. Why?

			for (int i = 0; i < 4; i++)
			{
				if (prepareArmor(pMob, i, f))
				{
					glColor4f(fBright, 0.0f, 0.0f, 0.4f);
					m_pArmorModel->render(x2, x1, fBob, aYaw - fSmth, aPitch, fScale);
				}
			}

		}
		if (GET_ALPHA(iOverlayColor))
		{
			float r = float(GET_RED(iOverlayColor)) / 255.0f;
			float g = float(GET_GREEN(iOverlayColor)) / 255.0f;
			float b = float(GET_BLUE(iOverlayColor)) / 255.0f;
			float aa = float(GET_ALPHA(iOverlayColor)) / 255.0f;
			glColor4f(r, g, b, aa);

			m_pModel->render(x2, x1, fBob, aYaw - fSmth, aPitch, fScale); // same here

			for (int i = 0; i < 4; i++)
			{
				if (prepareArmor(pMob, i, f))
				{
					glColor4f(r, g, b, aa);
					m_pArmorModel->render(x2, x1, fBob, aYaw - fSmth, aPitch, fScale);
				}
			}

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

void MobRenderer::renderName(Mob* mob, float x, float y, float z)
{
	if (!mob->isPlayer())
		return;

	Player* player = (Player*)mob;
	if (player == m_pDispatcher->m_pMinecraft->m_pLocalPlayer)
		return;

	// @TODO: don't know why but I have to add this correction. look into it and fix it!
	renderNameTag(mob, player->m_name, x, y - 1.5f, z, mob->isSneaking() ? 32 : 64);
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
	glRotatef(-m_pDispatcher->m_rot.x,   0.0f, 1.0f, 0.0f);
	glRotatef(+m_pDispatcher->m_rot.y, 1.0f, 0.0f, 0.0f);
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
