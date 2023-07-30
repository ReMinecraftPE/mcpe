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
	float getShadowHeightOffs() override;
	bool interact(Player* player) override;
	int interactPreventDefault() override;
	bool isPickable() override;
	bool isPushable() override;
	void tick() override;

public:
	int field_B8C = 0;
	int field_B90 = 80;
	Player* m_owner = nullptr;
	bool m_bActive = false;
};

