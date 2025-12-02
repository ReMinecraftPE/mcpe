/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "GameMods.hpp"

#ifdef ENH_ALLOW_SAND_GRAVITY

#include "EntityRenderer.hpp"
#include "../TileRenderer.hpp"

class FallingTileRenderer : public EntityRenderer
{
protected:
	class Materials
	{
	public:
		mce::MaterialPtr heavy_tile;

		Materials();
	};

public:
	FallingTileRenderer();

	void render(const Entity& entity, const Vec3& pos, float rot, float a) override;

protected:
	Materials m_heavyMaterials;
public:
	TileRenderer m_tileRenderer;
};

#endif
