/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Entity.hpp"

class PrimedTnt : public Entity
{
public:
	PrimedTnt(Level*);
	PrimedTnt(Level*, float, float, float);

	float getShadowHeightOffs() override;
	bool isPickable() override;
	void tick() override;

	void explode();

public:
	int m_fuseTimer;
};

