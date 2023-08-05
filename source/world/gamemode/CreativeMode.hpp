/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "GameMode.hpp"

class CreativeMode : public GameMode
{
public:
	CreativeMode(Minecraft*);

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

public:
	int m_destroyingX;
	int m_destroyingY;
	int m_destroyingZ;
	float field_18;
	float field_1C;
	int field_20;
	int field_24;
};

