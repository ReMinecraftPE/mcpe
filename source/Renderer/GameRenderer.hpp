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

class GameRenderer
{
public:
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

	float getFov(float f);

public:
	ItemInHandRenderer* m_pItemInHandRenderer = nullptr;
	Minecraft* m_pMinecraft = nullptr;

	float field_8 = 0.0f;
	int field_C = 0;
	Entity* field_10 = nullptr;
	float field_14 = 0.0f;
	float field_18 = 0.0f;
	float field_1C = 0.0f;
	float field_20 = 0.0f;
	float field_24 = 0.0f;
	float field_28 = 0.0f;
	float field_2C = 4.0f;
	float field_30 = 4.0f;
	float field_34 = 0.0f;
	float field_38 = 0.0f;
	float field_3C = 0.0f;
	float field_40 = 0.0f;
	float field_44 = 1.0f;
	float field_48 = 0.0f;
	float field_4C = 0.0f;
	float field_50 = 0.0f;
	float field_54 = 0.0f;
	float field_58 = 0.0f;
	float field_5C = 0.0f;
	float field_60 = 0.0f;
	float field_64 = 0.0f;
	float field_68 = 0.0f;
	float field_6C = 0.0f;
	float field_70 = 0.0f;
	float field_74 = 0.0f;
	float field_78 = 0.0f;
	float field_7C = 0.0f;
	float field_80 = 0.0f;
	float field_84 = 0.0f;

	float m_matrix_projection[16];
	float m_matrix_model_view[16];
};

