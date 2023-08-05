/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Entity.hpp"

class FallingTile : public Entity
{
public:
	FallingTile(Level*);
	FallingTile(Level*, float x, float y, float z, int id);

	float getShadowHeightOffs() override;
	bool isPickable() override;
	void tick() override;

	Level* getLevel();

public:
	int m_id;
	int field_E0 = 0;
};

