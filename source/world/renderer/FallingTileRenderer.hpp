/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once
#if defined(ENH_ALLOW_SAND_GRAVITY)

#include "EntityRenderer.hpp"
#include "TileRenderer.hpp"

class FallingTileRenderer : public EntityRenderer
{
public:
	FallingTileRenderer();

	void render(Entity*, float, float, float, float, float) override;

public:
	TileRenderer m_tileRenderer;
};

#endif
