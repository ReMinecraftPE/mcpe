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

	bool startDestroyBlock(Player* player, const TilePos& pos, Facing::Name face) override;
	bool destroyBlock(Player* player, const TilePos& pos, Facing::Name face) override;
	bool continueDestroyBlock(Player* player, const TilePos& pos, Facing::Name face) override;
	void stopDestroyBlock() override;
	void tick() override;
	void render(float f) override;
	float getBlockReachDistance() const override { return 4.0f; } // 4.0f on Java, 5.0f until 0.10.0-0.12.1
	float getEntityReachDistance() const override { return 3.0f; }
	bool useItemOn(Player*, Level*, ItemInstance*, const TilePos& pos, Facing::Name face) override;
	bool isCreativeType() const override { return false; }
	bool isSurvivalType() const override { return true; }
	void initPlayer(Player*) override;
	bool canHurtPlayer() override;
	float getDestroyModifier() const override { return 1.0; }

public:
	TilePos m_destroyingPos;
	float m_destroyProgress;
	float m_lastDestroyProgress;
	int m_destroyTicks;
	int m_destroyCooldown;
};

