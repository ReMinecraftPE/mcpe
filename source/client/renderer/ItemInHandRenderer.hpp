/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "world/item/ItemInstance.hpp"
#include "TileRenderer.hpp"

class Minecraft;

class ItemInHandRenderer
{
public:
	ItemInHandRenderer(Minecraft* pMC);
	void itemPlaced();
	void itemUsed();
	void render(float a);
	void renderItem(ItemInstance*);
	void renderScreenEffect(float f);
	void renderFire(float f);
	void renderTex(float f, int tex);
	void tick();
	void turn(const Vec2& rot);

private:
	int m_lastSlot;
	ItemInstance m_selectedItem;
	Minecraft* m_pMinecraft;
	int field_18;
	float m_height;
	float m_oHeight;
	TileRenderer m_tileRenderer;
};

