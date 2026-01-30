/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "EntityRenderer.hpp"
#include "client/model/geom/ModelPart.hpp"
#include "client/renderer/TileRenderer.hpp"
#include "world/tile/Tile.hpp"
#include "world/entity/TripodCamera.hpp"

class TripodCameraRenderer : public EntityRenderer
{
public:
	TripodCameraRenderer();

	void render(const Entity& entity, const Vec3& pos, float rot, float a) override;
	
	static float getFlashTime(const TripodCamera& camera, float f);

public:
	TileRenderer m_renderer;
	ModelPart m_modelPart;
};

