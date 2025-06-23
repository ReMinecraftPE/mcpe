/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "ItemInHandRenderer.hpp"

class Minecraft;
class Entity;

class LevelRenderer;
class GameRenderer
{
private:
	void _init();
public:
	GameRenderer() { _init(); }
	GameRenderer(Minecraft*);
	~GameRenderer();
	void saveMatrices();
	void setupCamera(float f, int i);
	void bobHurt(float);
	void bobView(float);
	void moveCameraToPlayer(float);

#ifndef ORIGINAL_CODE
	void renderNoCamera();
#endif

	void renderLevel(float);
	void render(float);
	void tick();
	void setupGuiScreen();
	void onGraphicsReset();
	void zoomRegion(float a, float b, float c);
	void unZoomRegion();
	void setupClearColor(float f);
	void setupFog(int i);
	void pick(float);
	void renderItemInHand(float, int);
	void prepareAndRenderClouds(LevelRenderer* pLR, float f);
	void renderWeather(float f);

	float getFov(float f);

public:
	ItemInHandRenderer* m_pItemInHandRenderer;
	Minecraft* m_pMinecraft;

	float field_8;
	int field_C;
	Entity* m_pHovered;
	float field_14;
	float field_18;
	float field_1C;
	float field_20;
	float field_24;
	float field_28;
	float field_2C;
	float field_30;
	float field_34;
	float field_38;
	float field_3C;
	float field_40;
	float field_44;
	float field_48;
	float field_4C;
	float field_50;
	float field_54;
	float field_58;
	float field_5C;
	float field_60;
	float field_64;
	float field_68;
	float field_6C;
	float field_70;
	float field_74;
	float field_78;
	float field_7C;
	float field_80;
	float field_84;

	float m_matrix_projection[16];
	float m_matrix_model_view[16];

	int m_shownFPS, m_shownChunkUpdates, m_lastUpdatedMS;

	int m_envTexturePresence;
	Random m_random;
};

