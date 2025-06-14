/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "CreeperRenderer.hpp"

CreeperRenderer::CreeperRenderer(Model* pModel, float f) : MobRenderer(pModel, f)
{
}

CreeperRenderer::~CreeperRenderer()
{
}

int CreeperRenderer::getOverlayColor(Mob* pMob, float a, float b)
{
	Creeper* pCreeper = (Creeper*)pMob;

	float step = pCreeper->getSwelling(b);

	if (static_cast<int>(step * 10.0f) % 2 == 0)
	{
		return 0;
	}

	int _a = step * 0.2f * 255.0f;

	if (_a < 0) { _a = 0; }

	if (_a > 255) { _a = 255; }

	int red = 255;
	int green = 255;
	int blue = 255;

	return _a << 24 | red << 16 | green << 8 | blue;
}

void CreeperRenderer::scale(Mob* pMob, float f)
{
	Creeper* pCreeper = (Creeper*)pMob;

	float g = pCreeper->getSwelling(f);
	float wobble = 1.0f + Mth::sin(g * 100.0f) * g * 0.01f;

	if (g < 0.0f) 
	{
		g = 0.0f;
	}

	if (g > 1.0f) 
	{
		g = 1.0f;
	}

	g *= g;
	g *= g;

	float s = (1.0f + g * 0.4f) * wobble;
	float hs = (1.0f + g * 0.1f) / wobble;

	glScalef(s, hs, s);
}
