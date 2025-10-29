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

void HumanoidMobRenderer::additionalRendering(Mob* mob, float f)
{
	ItemInstance* inst = mob->getCarriedItem();

	glPushMatrix();
	m_pHumanoidModel->m_arm1.translateTo(0.0625f);
	glTranslatef(-0.0625f, 0.4375f, 0.0625f);

	if (inst && inst->getTile() && TileRenderer::canRender(inst->getTile()->getRenderShape()))
	{
		glTranslatef(0.0f, 0.1875f, -0.3125f);
		glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
		glScalef(0.375f, -0.375f, 0.375f);
	}
	else if (inst && inst->getItem() && inst->getItem()->isHandEquipped())
	{
		glTranslatef(0.0f, 0.1875f, 0.0f);
		glScalef(0.625f, -0.625f, 0.625f);
		glRotatef(-100.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		glTranslatef(0.25f, 0.1875f, -0.1875f);
		glScalef(0.375f, 0.375f, 0.375f);
		glRotatef(60.0f, 0.0f, 0.0f, 1.0f);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(20.0f, 0.0f, 0.0f, 1.0f);
	}

	glEnable(GL_RESCALE_NORMAL);
	m_pDispatcher->m_pItemInHandRenderer->renderItem(inst);
	glPopMatrix();
	glDisable(GL_RESCALE_NORMAL);
}

void HumanoidMobRenderer::render(Entity* pEntity, const Vec3& pos, float f1, float f2)
{
	if (pEntity->isPlayer())
	{
		Player* player = (Player*)pEntity;
		ItemInstance* item = player->getSelectedItem();
		m_pHumanoidModel->m_bHoldingRightHand = item != nullptr;
	}

	if (pEntity->isSneaking())
	{
		m_pHumanoidModel->m_bSneaking = true;
		Vec3 pos2 = pos;
		pos2.y -= 0.125f;
		MobRenderer::render(pEntity, pos2, f1, f2);
		// https://github.com/ReMinecraftPE/mcpe/pull/197/#discussion_r2437985914
		m_pHumanoidModel->m_bSneaking = false;
	}
	else
	{
		MobRenderer::render(pEntity, pos, f1, f2);
	}
}

void HumanoidMobRenderer::onGraphicsReset()
{
	m_pHumanoidModel->onGraphicsReset();
}

void HumanoidMobRenderer::renderHand()
{
	m_pHumanoidModel->field_4 = 0;
	m_pHumanoidModel->setBrightness(m_pDispatcher->m_pMinecraft->m_pMobPersp->getBrightness(1.0f));
	m_pHumanoidModel->setupAnim(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0625f);
	m_pHumanoidModel->m_arm1.render(0.0625f);
}
