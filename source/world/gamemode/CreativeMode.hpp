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
	bool destroyBlock(int, int, int, int);

	float getPickRange() override;
	bool isCreativeType() override;
	bool isSurvivalType() override;
	void initPlayer(Player*) override;
};

