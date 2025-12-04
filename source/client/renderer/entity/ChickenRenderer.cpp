/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "ChickenRenderer.hpp"
#include "world/entity/Chicken.hpp"

ChickenRenderer::ChickenRenderer(Model* pModel, float f) : MobRenderer(pModel, f)
{
}

ChickenRenderer::~ChickenRenderer()
{
}

float ChickenRenderer::getBob(const Mob& mob, float time)
{
	const Chicken& chicken = (const Chicken&)mob;

	return (float)((Mth::sin((chicken.m_oFlap + (float)((float)(chicken.m_flap - chicken.m_oFlap) * time))/*, *(float*)&chicken->m_flap*/))
		+ 1.0)
		* chicken.m_oFlapSpeed + (float)((float)(chicken.m_flapSpeed - chicken.m_oFlapSpeed) * time);
}
