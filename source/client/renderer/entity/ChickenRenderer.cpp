/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "ChickenRenderer.hpp"

ChickenRenderer::ChickenRenderer(Model* pModel, float f) : MobRenderer(pModel, f)
{
}

ChickenRenderer::~ChickenRenderer()
{
}

float ChickenRenderer::getBob(Mob*, float)
{
	// TODO
	return 0.0f;
}
