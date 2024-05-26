/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Mob.hpp"

class Level;
class Player;

class TripodCamera : public Mob
{
public:
	TripodCamera(Level*, Player*, float, float, float);

	void defineSynchedData() override;
	float getShadowHeightOffs() const override { return 0.0f; }
	bool interact(Player* player) override;
	int interactPreventDefault() override;
	bool isPickable() const override { return !m_bRemoved; }
	bool isPushable() const override { return false; }
	void tick() override;

public:
	int field_B8C;
	int field_B90;
	Player* m_owner;
	bool m_bActive;
};

