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
	void render(float f);
	void renderItem(ItemInstance*);
	void renderScreenEffect(float f);
	void renderFire(float f);
	void renderTex(float f, int tex);
	void tick();
	void turn(float yd, float pd);

public:
	int field_0;
	ItemInstance m_ItemInstance;
	Minecraft* m_pMinecraft;
	int field_18;
	float field_1C;
	float field_20;
	TileRenderer m_tileRenderer;
	float m_yawOffsVel;
	float m_pitchOffsVel;
	float m_yawOffs;
	float m_pitchOffs;
	float m_lastYawOffs;
	float m_lastPitchOffs;
};

