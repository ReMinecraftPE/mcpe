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
#include "world/item/ArmorItem.hpp"

HumanoidMobRenderer::HumanoidMobRenderer(HumanoidModel* pModel, float f) : MobRenderer(pModel, f)
{
	m_pHumanoidModel = pModel;
	m_armorMain = new HumanoidModel(1.0f);
	m_armorLeggings = new HumanoidModel(0.5f);
	m_armorMain->m_pMaterial = &m_armorMain->m_materials.entity_alphatest;
	m_armorLeggings->m_pMaterial = &m_armorLeggings->m_materials.entity_alphatest;
}

int HumanoidMobRenderer::prepareArmor(const Mob& mob, int layer, float a)
{
	// If other humanoid mobs get armor/inventories, this can be changed
	// .wearsArmor()?
	// m_pInventory->getArmor(...) => .getArmorSlot(...)?
	if (!mob.isPlayer())
		return false;
	
	ItemStack inst = static_cast<const Player&>(mob).m_pInventory->getArmor(static_cast<Item::EquipmentSlot>(3 - layer));
	// Mob isn't wearing armor in the current slot
	if (inst.isEmpty())
		return false;

	const std::string& texture = inst.getItem()->getArmorTexture();
	if (texture.empty()) 
		return false;

	bindTexture(texture);
	
	HumanoidModel* armor = (layer == 2) ? m_armorLeggings : m_armorMain;

	// Head: pass 0
	armor->m_head.m_bVisible = layer == 0;
	armor->m_hat.m_bVisible = layer == 0;
	
	// Body: passes 1 & 2
	armor->m_body.m_bVisible = layer == 1 || layer == 2;

	// Arms: pass 1
	armor->m_armRight.m_bVisible = layer == 1;
	armor->m_armLeft.m_bVisible = layer == 1;

	// Leggings: Passes 2 and 3
	armor->m_legRight.m_bVisible = layer == 2 || layer == 3;
	armor->m_legLeft.m_bVisible = layer == 2 || layer == 3;
	
	setArmor(armor);

	return true;
}

void HumanoidMobRenderer::additionalRendering(const Mob& mob, float f)
{
	constexpr float fScale = 0.625f;

	if (mob.isPlayer())
	{
		const Player& player = (const Player&)mob;

		ItemStack& headGear = player.m_pInventory->getArmor(Item::SLOT_HEAD);

		// Tile hat
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

	m_pHumanoidModel->m_armRight.translateTo(matrix, 0.0625f);
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
		m_armorMain->m_bHoldingRightHand = m_armorLeggings->m_bHoldingRightHand = m_pHumanoidModel->m_bHoldingRightHand = !item.isEmpty();
		bSetRHolding = true;
	}

	if (entity.isSneaking())
	{
		m_armorMain->m_bSneaking = m_armorLeggings->m_bSneaking = m_pHumanoidModel->m_bSneaking = true;
		Vec3 pos2 = pos;
		pos2.y -= 0.125f;
		MobRenderer::render(entity, pos2, f1, f2);
		// https://github.com/ReMinecraftPE/mcpe/pull/197/#discussion_r2437985914
		m_armorMain->m_bSneaking = m_armorLeggings->m_bSneaking = m_pHumanoidModel->m_bSneaking = false;
	}
	else
	{
		MobRenderer::render(entity, pos, f1, f2);
	}

	if (bSetRHolding)
	{
		// Fix the weird first-person hand orientation bug
		m_armorMain->m_bHoldingRightHand = m_armorLeggings->m_bHoldingRightHand = m_pHumanoidModel->m_bHoldingRightHand = false;
	}
}

void HumanoidMobRenderer::onGraphicsReset()
{
	m_pHumanoidModel->onGraphicsReset();
}

void HumanoidMobRenderer::renderHand(const Entity& entity, float a)
{
	float brightness = m_pDispatcher->m_pMinecraft->m_pCameraEntity->getBrightness(1.0f);
	m_pHumanoidModel->m_attackTime = 0;
	m_pHumanoidModel->setBrightness(brightness);
	m_pHumanoidModel->setupAnim(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0625f);

	_setupShaderParameters(entity, a);
	m_pHumanoidModel->m_armRight.render(0.0625f);

#ifdef ENH_RENDER_HAND_ARMOR
	if (entity.isPlayer())
	{
		ItemStack inst = static_cast<const Player&>(entity).m_pInventory->getArmor(Item::SLOT_CHEST);
		if (!inst.isEmpty())
		{
			const std::string& texture = inst.getItem()->getArmorTexture();
			if (!texture.empty())
			{
				bindTexture(texture);
				
				m_armorMain->m_attackTime = 0;
				m_armorMain->m_armRight.m_bVisible = true;
				
				m_armorMain->setBrightness(brightness);
				m_armorMain->setupAnim(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0625f);

				m_armorMain->m_armRight.render(0.0625f);
			}
		}
	}
#endif
}

void HumanoidMobRenderer::scale(const Mob &mob, Matrix &matrix, float a)
{
	// players are actually 15/16ths the size than any other bipedal mob
	matrix.scale((mob.isPlayer()) ? (15.0f / 16.0f) : 1.0f);
}
