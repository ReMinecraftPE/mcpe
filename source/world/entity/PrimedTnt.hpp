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
private:
	void _init();
public:
	PrimedTnt(Level* level) : Entity(level) { _init(); }
	PrimedTnt(Level* level, float x, float y, float z);

	float getShadowHeightOffs() const override;
	bool isPickable() const override;
	void tick() override;

	void explode();

public:
	int m_fuseTimer;
};

