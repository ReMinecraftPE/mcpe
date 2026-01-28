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
#include "world/item/ItemStack.hpp"

HumanoidMobRenderer::HumanoidMobRenderer(HumanoidModel* pModel, float f) : MobRenderer(pModel, f)
{
	m_pHumanoidModel = pModel;
}

void HumanoidMobRenderer::additionalRendering(const Mob& mob, float f)
{
	constexpr float fScale = 0.625f;

	if (mob.isPlayer())
	{
		const Player& player = (const Player&)mob;

		ItemStack& headGear = player.m_pInventory->getArmor(Item::SLOT_HEAD);
		if (!headGear.isEmpty() && headGear.getItem()->m_itemID < C_MAX_TILES)
		{
			MatrixStack::Ref matrix = MatrixStack::World.push();
			m_pHumanoidModel->m_head.translateTo(matrix, 0.0625f);
			if (TileRenderer::canRender(Tile::tiles[headGear.getId()]->getRenderShape()))
			{
				matrix->translate(Vec3(0.0f, -0.25f, 0.0f));
				matrix->rotate(180.0f, Vec3(0.0f, 1.0f, 0.0f));
				matrix->scale(Vec3(fScale, -fScale, fScale));
			}

			m_pDispatcher->m_pItemInHandRenderer->renderItem(mob, headGear, f);
		}
	}

	const ItemStack& item = mob.getCarriedItem();

	MatrixStack::Ref matrix = MatrixStack::World.push();

	m_pHumanoidModel->m_arm1.translateTo(matrix, 0.0625f);
	matrix->translate(Vec3(-0.0625f, 0.4375f, 0.0625f));

	if (!item.isEmpty() && item.getTile() && TileRenderer::canRender(item.getTile()->getRenderShape()))
	{
		constexpr float s = 0.5f * 0.75f;
		matrix->translate(Vec3(0.0f, 0.1875f, -0.3125f));
		matrix->rotate(200.0f, Vec3::UNIT_X);
		matrix->rotate(45.0f, Vec3::UNIT_Y);
		matrix->scale(s);
	}
	else if (!item.isEmpty() && item.getItem() && item.getItem()->isHandEquipped())
	{
		matrix->rotate(180.0f, Vec3::UNIT_Y);
		// PE's fucked up translation value
		//matrix->translate(Vec3(0.1f, 0.265f, 0.0f));
		matrix->translate(Vec3(0.06f, 0.1875f, 0.0f));
		matrix->scale(fScale);
		matrix->rotate(80.0f, Vec3::UNIT_X);
		matrix->rotate(35.0f, Vec3::UNIT_Y);
	}
	else
	{
		constexpr float s = 0.375f;
		matrix->translate(Vec3(0.25f, 0.1875f, -0.1875f));
		matrix->scale(s);
		matrix->rotate(60.0f, Vec3::UNIT_Z);
		matrix->rotate(-90.0f, Vec3::UNIT_X);
		matrix->rotate(20.0f, Vec3::UNIT_Z);
	}

	if (!item.isEmpty())
	{
		m_pDispatcher->m_pItemInHandRenderer->renderItem(mob, item, f);
	}
}

void HumanoidMobRenderer::render(const Entity& entity, const Vec3& pos, float f1, float f2)
{
	bool bSetRHolding = false;

	if (entity.isPlayer())
	{
		const Player& player = (const Player&)entity;
		ItemStack& item = player.getSelectedItem();
		m_pHumanoidModel->m_bHoldingRightHand = !item.isEmpty();
		bSetRHolding = true;
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

	if (bSetRHolding)
	{
		// Fix the weird first-person hand orientation bug
		m_pHumanoidModel->m_bHoldingRightHand = false;
	}
}

void HumanoidMobRenderer::onGraphicsReset()
{
	m_pHumanoidModel->onGraphicsReset();
}

void HumanoidMobRenderer::setupPosition(const Entity& entity, const Vec3& pos, Matrix& matrix)
{
	if (entity.isAlive() && entity.isMob())
	{
		const Mob& mob = (const Mob&)entity;
		if (mob.isSleeping() && entity.isPlayer())
		{
			const Player& player = (const Player&)entity;
			Vec3 sleepPos(
				pos.x + player.m_sleepingPos.x,
				pos.y,
				pos.z + player.m_sleepingPos.z
			);
			MobRenderer::setupPosition(entity, sleepPos, matrix);
			return;
		}
	}
	MobRenderer::setupPosition(entity, pos, matrix);
}

void HumanoidMobRenderer::setupRotations(const Entity& entity, float bob, float bodyRot, Matrix& matrix, float a)
{
	if (entity.isAlive() && entity.isMob())
	{
		const Mob& mob = (const Mob&)entity;
		if (mob.isSleeping() && entity.isPlayer())
		{
			const Player& player = (const Player&)entity;
			if (player.m_pLevel)
			{
				matrix.rotate(player.getBedSleepRot(), Vec3::UNIT_Y);
				matrix.rotate(getFlipDegrees(mob), Vec3::UNIT_Z);
				matrix.rotate(270.0f, Vec3::UNIT_Y);
				return;
			}
		}
	}
	MobRenderer::setupRotations(entity, bob, bodyRot, matrix, a);
}

void HumanoidMobRenderer::renderHand(const Entity& entity, float a)
{
	m_pHumanoidModel->field_4 = 0;
	m_pHumanoidModel->setBrightness(m_pDispatcher->m_pMinecraft->m_pCameraEntity->getBrightness(1.0f));
	m_pHumanoidModel->setupAnim(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0625f);

	_setupShaderParameters(entity, a);
	m_pHumanoidModel->m_arm1.render(0.0625f);
}
