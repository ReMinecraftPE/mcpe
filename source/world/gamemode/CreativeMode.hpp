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
	CreativeMode(Minecraft*, Level&);

	//float getBlockReachDistance() const override { return 5.0f; } // 5.0f on Java
	bool isCreativeType() const override { return true; }

	void initPlayer(Player*) override;
	bool destroyBlock(Player* player, const TilePos& pos, Facing::Name face) override;
	bool startDestroyBlock(Player* player, const TilePos& pos, Facing::Name face) override;
	bool continueDestroyBlock(Player* player, const TilePos& pos, Facing::Name face) override;
	void stopDestroyBlock() override;
	void tick() override;
	void render(float f) override;
	float getDestroyModifier() const override { return 8.0; }

public:
	TilePos m_destroyingPos;
	float m_destroyProgress;
	float m_lastDestroyProgress;
	int m_destroyTicks;
	int m_destroyCooldown;
};

