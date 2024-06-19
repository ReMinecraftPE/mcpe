/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "ITurnInput.hpp"
#include "IBuildInput.hpp"
#include "IInputHolder.hpp"
#include "common/SmoothFloat.hpp"
#include "RectangleArea.hpp"
#include "IncludeExcludeArea.hpp"
#include "TouchAreaModel.hpp"

class Entity;

class UnifiedTurnBuild : public ITurnInput, public IBuildInput
{
public:
	UnifiedTurnBuild(int, int width, int height, float, float, IInputHolder*);

	// ITurnInput
	void setScreenSize(int width, int height) override; // actually IBuildInput also has it
	TurnDelta getTurnDelta() override;
	bool smoothTurning() override;

	// IBuildInput
	bool tickBuild(Player*, BuildActionIntention*) override;

	bool isInsideArea(float x, float y);

	static float getSpeedSquared(Entity* pEnt);

public:
	int field_C;
	int field_10;
	SmoothFloat m_smoothFloat;
	bool field_24;
	RectangleArea m_screenArea;
	RectangleArea field_40;
	RectangleArea field_58;
	IInputHolder* m_pInputHolder;
	float field_78;
	float field_7C;
	bool m_bWasTouched;
	TouchAreaModel m_touchAreaModel;
	IncludeExcludeArea m_includeExcludeArea;
	float field_B8;
	float field_BC;
	float field_C0;
	float field_C4;
	Entity* m_pPlayer;
	float field_CC;
	int field_D0;
	bool field_D4;
	int field_D8;
};

