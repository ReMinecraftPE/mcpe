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

	int getOverlayColor(Mob*, float, float) override;
#ifdef ENH_GFX_MATRIX_STACK
	void scale(Mob* pMob, Matrix& matrix, float a) override;
#else
	void scale(Mob* pMob, float a) override;
#endif
};
