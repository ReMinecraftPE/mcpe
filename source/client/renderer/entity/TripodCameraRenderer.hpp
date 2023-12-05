/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "EntityRenderer.hpp"
#include "client/renderer/TileRenderer.hpp"
#include "world/tile/Tile.hpp"
#include "world/entity/TripodCamera.hpp"

class TripodTile : public Tile
{
public:
	TripodTile();
	int getRenderShape() override;
};

class TripodCameraRenderer : public EntityRenderer
{
public:
	TripodCameraRenderer();

	void render(Entity*, float, float, float, float, float) override;
	
	static float getFlashTime(TripodCamera*, float f);

public:
	TileRenderer m_renderer;
	TripodTile m_tile;
	ModelPart m_modelPart;
};

