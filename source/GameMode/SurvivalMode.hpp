/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "GameMode.hpp"

class SurvivalMode : public GameMode
{
public:
	SurvivalMode(Minecraft*);

	virtual void startDestroyBlock(int x, int y, int z, int i) override;
	virtual bool destroyBlock(int x, int y, int z, int i);
	virtual void continueDestroyBlock(int x, int y, int z, int i);
	virtual void stopDestroyBlock() override;
	virtual void tick() override;
	virtual void render(float f) override;
	virtual float getPickRange() override;
	virtual bool isCreativeType() override;
	virtual bool isSurvivalType() override;
	virtual void initPlayer(Player*) override;

public:
	int m_destroyingX = -1;
	int m_destroyingY = -1;
	int m_destroyingZ = -1;
	float field_18 = 0.0f;
	float field_1C = 0.0f;
	int field_20 = 0;
	int field_24 = 0;
};

