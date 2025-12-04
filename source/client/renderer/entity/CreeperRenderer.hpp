/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "MobRenderer.hpp"
#include "world/entity/Creeper.hpp"

class CreeperRenderer : public MobRenderer
{
public:
	CreeperRenderer(Model*, float);
	~CreeperRenderer();

	Color getOverlayColor(const Entity& entity, float a) override;
	void scale(const Mob& mob, Matrix& matrix, float a) override;
};
