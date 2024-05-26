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

class FakeRocketTile : public Tile
{
public:
	FakeRocketTile();
	int getRenderShape() const override;
};

class RocketRenderer : public EntityRenderer
{
public:
	RocketRenderer();
	void render(Entity*, float, float, float, float, float) override;

public:
	TileRenderer m_renderer;
	FakeRocketTile m_tile;
};

