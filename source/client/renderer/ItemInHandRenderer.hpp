/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "client/app/AppPlatformListener.hpp"
#include "client/renderer/renderer/EntityShaderManager.hpp"
#include "world/item/ItemStack.hpp"
#include "TileRenderer.hpp"

class Minecraft;

class ItemInHandRenderer : public EntityShaderManager, public AppPlatformListener
{
protected:
	class Materials
	{
	public:
		mce::MaterialPtr entity;
		mce::MaterialPtr entity_alphatest;
		mce::MaterialPtr item_in_hand;
		mce::MaterialPtr item_in_hand_color;
		mce::MaterialPtr entity_glint;
		mce::MaterialPtr entity_alphatest_glint;
		mce::MaterialPtr item_in_hand_glint;

		Materials();
	};

public:
	ItemInHandRenderer(Minecraft* pMC);
	void itemPlaced();
	void itemUsed();
	void render(float a);
	void renderItem(const Entity& entity, const ItemStack& item, float a);
	void renderScreenEffect(float a);
	void renderWater(float a);
	void renderFire(float a);
	void renderTex(float a, int tex);
	void tick();
	void turn(const Vec2& rot);

private:
	int m_lastSlot;
	ItemStack m_selectedItem;
	Minecraft* m_pMinecraft;
	int field_18;
	float m_height;
	float m_oHeight;
	TileRenderer m_tileRenderer;
	Materials m_materials;
};

