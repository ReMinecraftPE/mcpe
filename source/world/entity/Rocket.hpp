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

class Rocket : public Entity
{
public:
	Rocket(Level*, float, float, float);

	float getShadowHeightOffs() const override;
	bool interact(Player* player) override;
	int interactPreventDefault() override;
	bool isPickable() const override;
	bool isPushable() const override;
	void tick() override;

public:
	int field_B8C;
	int field_B90;
};

