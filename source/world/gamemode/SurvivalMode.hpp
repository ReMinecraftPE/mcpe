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
	SurvivalMode(Minecraft*, Level&);

	void startDestroyBlock(int x, int y, int z, int i) override;
	bool destroyBlock(int x, int y, int z, int i) override;
	void continueDestroyBlock(int x, int y, int z, int i) override;
	void stopDestroyBlock() override;
	void tick() override;
	void render(float f) override;
	float getPickRange() const override { return 5.0f; }
	bool isCreativeType() const override { return false; }
	bool isSurvivalType() const override { return true; }
	void initPlayer(Player*) override;
	bool canHurtPlayer() override;
	float getDestroyModifier() const override { return 1.0; }

public:
	int m_destroyingX;
	int m_destroyingY;
	int m_destroyingZ;
	float m_destroyProgress;
	float m_lastDestroyProgress;
	int m_destroyTicks;
	int m_destroyCooldown;
};

