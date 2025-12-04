/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <sstream>

#include "MobRenderer.hpp"
#include "client/app/Minecraft.hpp"
#include "renderer/ShaderConstants.hpp"
#include "EntityRenderDispatcher.hpp"

MobRenderer::MobRenderer(Model* pModel, float f)
{
	m_pArmorModel = nullptr;
	m_pModel = pModel;
	m_shadowRadius = f;
}

MobRenderer::~MobRenderer()
{
    SAFE_DELETE(m_pModel);
    SAFE_DELETE(m_pArmorModel);
}

void MobRenderer::setArmor(Model* model)
{
	m_pArmorModel = model;
}

int MobRenderer::prepareArmor(const Mob& mob, int a, float b)
{
	return 0;
}

void MobRenderer::additionalRendering(const Mob& mob, float f)
{
}

float MobRenderer::getAttackAnim(const Mob& mob, float f)
{
	return mob.getAttackAnim(f);
}

float MobRenderer::getBob(const Mob& mob, float f)
{
	return float(mob.m_tickCount) + f;
}

float MobRenderer::getFlipDegrees(const Mob& mob)
{
	return 90.0f;
}

void MobRenderer::setupPosition(const Entity& entity, const Vec3& pos, Matrix& matrix)
{
	// @HACK: I eye-balled a corrective offset of 1/13, since I still can't figure out why all mobs are floating - Brent
	// This was due to "0.059375f" being used for scale instead of "0.0625f"
	matrix.translate(pos);
}

void MobRenderer::setupRotations(const Entity& entity, float bob, float bodyRot, Matrix& matrix, float a)
{
	matrix.rotate(180.0f - bodyRot, Vec3::UNIT_Y);

	const Mob& mob = (const Mob&)entity;
	if (mob.m_deathTime > 0)
	{
		float t = Mth::sqrt((float(mob.m_deathTime) + a - 1.0f) / 20.0f * 1.6f);
		if (t > 1.0f)
			t = 1.0f;

		matrix.rotate(getFlipDegrees(mob) * t, Vec3::UNIT_Z);
	}
}

void MobRenderer::scale(const Mob& mob, Matrix& matrix, float a)
{

}

void MobRenderer::render(const Entity& entity, const Vec3& pos, float unused, float f)
{
	const Mob& mob = (const Mob&)entity;

	{
		MatrixStack::Ref matrix = MatrixStack::World.push();

		//glDisable(GL_CULL_FACE);

		m_pModel->field_4 = getAttackAnim(mob, f);
		m_pModel->m_bRiding = false;
		m_pModel->m_bIsBaby = mob.isBaby();

		if (m_pArmorModel != nullptr)
		{
			m_pArmorModel->m_bRiding = m_pModel->m_bRiding;
			m_pArmorModel->m_bIsBaby = m_pModel->m_bIsBaby;
		}

		float aYaw = mob.m_oRot.x + (mob.m_rot.x - mob.m_oRot.x) * f;
		float aPitch = mob.m_oRot.y + (mob.m_rot.y - mob.m_oRot.y) * f;
		float fBob = getBob(mob, f);
		float fSmth = mob.field_EC + (mob.field_E8 - mob.field_EC) * f;

		setupPosition(mob, Vec3(pos.x, pos.y - mob.m_heightOffset, pos.z), matrix);
		setupRotations(mob, fBob, fSmth, matrix, f);

		constexpr float fScale = 0.0625f; // the scale variable according to b1.2_02
		matrix->scale(Vec3(-1.0f, -1.0f, 1.0f));
		scale(mob, matrix, f);
		matrix->translate(Vec3(0.0f, -24.0f * fScale - (1.0f / 128.0f), 0.0f));

		float x1 = mob.field_128 + (mob.m_walkAnimSpeed - mob.field_128) * f;
		if (x1 > 1.0f)
			x1 = 1.0f;
		float x2 = mob.field_130 - mob.m_walkAnimSpeed * (1.0f - f);

		bindTexture(mob.getTexture());
#ifndef FEATURE_GFX_SHADERS
		//glEnable(GL_ALPHA_TEST);
#endif

		m_pModel->setBrightness(entity.getBrightness(1.0f));
		m_pModel->prepareMobModel(mob, x2, x1, f);
		m_pModel->render(x2, x1, fBob, aYaw - fSmth, aPitch, fScale);

		for (int i = 0; i < 4; i++)
		{
			if (prepareArmor(mob, i, f))
			{
				m_pArmorModel->render(x2, x1, fBob, aYaw - fSmth, aPitch, fScale);
				//glDisable(GL_BLEND);
#ifndef FEATURE_GFX_SHADERS
				//glEnable(GL_ALPHA_TEST);
#endif
			}
		}

		additionalRendering(mob, f);

		Color overlayColor = getOverlayColor(mob, f);
		if (overlayColor.a > 0)
		{
#ifndef FEATURE_GFX_SHADERS
			//glDisable(GL_TEXTURE_2D);
			//glDisable(GL_ALPHA_TEST);
#endif
			//glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // default
			//glDepthFunc(GL_EQUAL);

			currentShaderColor = overlayColor; //glColor4f(overlayColor.r, overlayColor.g, overlayColor.b, overlayColor.a);
			mce::MaterialPtr* pMaterial = m_pModel->m_pMaterial;
			m_pModel->m_pMaterial = &m_pModel->m_materials.entity_color_overlay;

			m_pModel->render(x2, x1, fBob, aYaw - fSmth, aPitch, fScale); // same here

			for (int i = 0; i < 4; i++)
			{
				if (prepareArmor(mob, i, f))
				{
					mce::MaterialPtr* pMaterial = m_pArmorModel->m_pMaterial;
					m_pArmorModel->m_pMaterial = &m_pArmorModel->m_materials.entity_color_overlay;

					currentShaderColor = overlayColor; //glColor4f(overlayColor.r, overlayColor.g, overlayColor.b, overlayColor.a);
					m_pArmorModel->render(x2, x1, fBob, aYaw - fSmth, aPitch, fScale);

					m_pArmorModel->m_pMaterial = pMaterial;
				}
			}

			m_pModel->m_pMaterial = pMaterial;

			//glDepthFunc(GL_LEQUAL);
			//glDisable(GL_BLEND);
#ifndef FEATURE_GFX_SHADERS
			//glEnable(GL_TEXTURE_2D);
			//glEnable(GL_ALPHA_TEST);
#endif
		}

		//glEnable(GL_CULL_FACE);
	}
	renderName(mob, pos);
}

void MobRenderer::onGraphicsReset()
{
	m_pModel->onGraphicsReset();
}

void MobRenderer::renderName(const Mob& mob, const Vec3& pos)
{
	if (mob.isPlayer())
	{
		const Player& player = (const Player&)mob;
		if (&player == m_pDispatcher->m_pMinecraft->m_pLocalPlayer)
			return;

		// @TODO: don't know why but I have to add this correction. look into it and fix it!
		renderNameTag(mob, player.m_name, Vec3(pos.x, pos.y - 1.5f, pos.z), mob.isSneaking() ? 32 : 64);
	}
	else
	{
		if (m_pDispatcher->m_pOptions->m_bDebugText)
		{
			std::stringstream ss;
			ss << mob.m_EntityID;
			renderNameTag(mob, ss.str(), pos, 64);
		}
	}
}

void MobRenderer::renderNameTag(const Mob& mob, const std::string& str, const Vec3& pos, int a)
{
	if (mob.distanceToSqr(m_pDispatcher->m_pCamera) > float(a * a))
		return;

	Font* font = getFont();

	MatrixStack::Ref matrix = MatrixStack::World.push();
	matrix->translate(Vec3(pos.x + 0.0f, pos.y + 2.3f, pos.z));
#ifndef __EMSCRIPTEN__
	glNormal3f(0.0f, 1.0f, 0.0f);
#endif
	// billboard the name towards the camera
	matrix->rotate(-m_pDispatcher->m_rot.x, Vec3::UNIT_Y);
	matrix->rotate(+m_pDispatcher->m_rot.y, Vec3::UNIT_X);
	matrix->scale(Vec3(-0.026667f, -0.026667f, 0.026667f));

	//glDepthMask(false);
	//glDisable(GL_DEPTH_TEST);

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // default
#ifndef FEATURE_GFX_SHADERS
	//glDisable(GL_TEXTURE_2D);
#endif
	
	currentShaderColor = Color(0.0f, 0.0f, 0.0f, 0.25f);

	Tesselator& t = Tesselator::instance;
	t.begin();

	int width = font->width(str);
	float widthHalf = float(width / 2);

	t.vertex(-1.0f - widthHalf, -1.0f, 0.0f);
	t.vertex(-1.0f - widthHalf, 8.0f, 0.0f);
	t.vertex(widthHalf + 1.0f, 8.0f, 0.0f);
	t.vertex(widthHalf + 1.0f, -1.0f, 0.0f);
	t.draw(m_materials.name_tag);

#ifndef FEATURE_GFX_SHADERS
	//glEnable(GL_TEXTURE_2D);
#endif

	font->draw(str, -font->width(str) / 2, 0, 0x20FFFFFF);
	//glEnable(GL_DEPTH_TEST);
	//glDepthMask(true);

	font->draw(str, -font->width(str) / 2, 0, 0xFFFFFFFF);

	//glDisable(GL_BLEND);
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}
