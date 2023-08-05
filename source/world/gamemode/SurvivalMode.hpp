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

	void startDestroyBlock(int x, int y, int z, int i) override;
	bool destroyBlock(int x, int y, int z, int i);
	void continueDestroyBlock(int x, int y, int z, int i);
	void stopDestroyBlock() override;
	void tick() override;
	void render(float f) override;
	float getPickRange() override;
	bool isCreativeType() override;
	bool isSurvivalType() override;
	void initPlayer(Player*) override;
	bool canHurtPlayer() override;

public:
	int m_destroyingX = -1;
	int m_destroyingY = -1;
	int m_destroyingZ = -1;
	float field_18 = 0.0f;
	float field_1C = 0.0f;
	int field_20 = 0;
	int field_24 = 0;
};

