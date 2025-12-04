/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "HumanoidMobRenderer.hpp"
#include "EntityRenderDispatcher.hpp"
#include "client/app/Minecraft.hpp"
#include "client/renderer/ItemInHandRenderer.hpp"
#include "client/renderer/TileRenderer.hpp"
#include "world/entity/Player.hpp"
#include "world/item/ItemInstance.hpp"

HumanoidMobRenderer::HumanoidMobRenderer(HumanoidModel* pModel, float f) : MobRenderer(pModel, f)
{
	m_pHumanoidModel = pModel;
}

void HumanoidMobRenderer::additionalRendering(const Mob& mob, float f)
{
	const ItemInstance* inst = mob.getCarriedItem();

#ifdef ENH_GFX_MATRIX_STACK
	MatrixStack::Ref matrix = MatrixStack::World.push();
#else
	glPushMatrix();
#endif

#ifdef ENH_GFX_MATRIX_STACK
	m_pHumanoidModel->m_arm1.translateTo(matrix, 0.0625f);
	matrix->translate(Vec3(-0.0625f, 0.4375f, 0.0625f));
#else
	m_pHumanoidModel->m_arm1.translateTo(0.0625f);
	glTranslatef(-0.0625f, 0.4375f, 0.0625f);
#endif

	if (inst && inst->getTile() && TileRenderer::canRender(inst->getTile()->getRenderShape()))
	{
		constexpr float s = 0.5f * 0.75f;
#ifdef ENH_GFX_MATRIX_STACK
		matrix->translate(Vec3(0.0f, 0.1875f, -0.3125f));
		matrix->rotate(200.0f, Vec3::UNIT_X);
		matrix->rotate(45.0f, Vec3::UNIT_Y);
		matrix->scale(s);
#else
		glTranslatef(0.0f, 0.1875f, -0.3125f);
		glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
		glScalef(s, -s, s);
#endif
	}
	else if (inst && inst->getItem() && inst->getItem()->isHandEquipped())
	{
		constexpr float s = 0.625f;
#ifdef ENH_GFX_MATRIX_STACK
		matrix->translate(Vec3(0.0f, 0.1875f, 0.0f));
		matrix->scale(s);
		matrix->rotate(80.0f, Vec3::UNIT_X);
		matrix->rotate(45.0f, Vec3::UNIT_Y);
#else
		glTranslatef(0.0f, 0.1875f, 0.0f);
		glScalef(s, -s, s);
		glRotatef(-100.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
#endif
	}
	else
	{
		constexpr float s = 0.375;
#ifdef ENH_GFX_MATRIX_STACK
		matrix->translate(Vec3(0.25f, 0.1875f, -0.1875f));
		matrix->scale(s);
		matrix->rotate(60.0f, Vec3::UNIT_Z);
		matrix->rotate(-90.0f, Vec3::UNIT_X);
		matrix->rotate(20.0f, Vec3::UNIT_Z);
#else
		glTranslatef(0.25f, 0.1875f, -0.1875f);
		glScalef(s, s, s);
		glRotatef(60.0f, 0.0f, 0.0f, 1.0f);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(20.0f, 0.0f, 0.0f, 1.0f);
#endif
	}

	m_pDispatcher->m_pItemInHandRenderer->renderItem(inst);

#ifndef ENH_GFX_MATRIX_STACK
	glPopMatrix();
#endif
}

void HumanoidMobRenderer::render(const Entity& entity, const Vec3& pos, float f1, float f2)
{
	if (entity.isPlayer())
	{
		const Player& player = (const Player&)entity;
		ItemInstance* item = player.getSelectedItem();
		m_pHumanoidModel->m_bHoldingRightHand = item != nullptr;
	}

	if (entity.isSneaking())
	{
		m_pHumanoidModel->m_bSneaking = true;
		Vec3 pos2 = pos;
		pos2.y -= 0.125f;
		MobRenderer::render(entity, pos2, f1, f2);
		// https://github.com/ReMinecraftPE/mcpe/pull/197/#discussion_r2437985914
		m_pHumanoidModel->m_bSneaking = false;
	}
	else
	{
		MobRenderer::render(entity, pos, f1, f2);
	}
}

void HumanoidMobRenderer::onGraphicsReset()
{
	m_pHumanoidModel->onGraphicsReset();
}

void HumanoidMobRenderer::renderHand()
{
	m_pHumanoidModel->field_4 = 0;
	m_pHumanoidModel->setBrightness(m_pDispatcher->m_pMinecraft->m_pCameraEntity->getBrightness(1.0f));
	m_pHumanoidModel->setupAnim(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0625f);
	m_pHumanoidModel->m_arm1.render(0.0625f);
}
