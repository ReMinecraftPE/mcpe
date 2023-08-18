/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "world/phys/AABB.hpp"
#include "client/model/HumanoidModel.hpp"
#include "client/renderer/Font.hpp"

class EntityRenderDispatcher;

class EntityRenderer
{
public:
	EntityRenderer();
	void bindTexture(const std::string& file);
	Font* getFont();
	void init(EntityRenderDispatcher* d);
	static void render(const AABB&, float, float, float);
	static void renderFlat(const AABB&);

	virtual void render(Entity*, float, float, float, float, float) = 0;
	virtual void onGraphicsReset();

public:
	float field_4;
	float field_8;
	EntityRenderDispatcher* m_pDispatcher;

	// @HUH: Why is there a HumanoidModel here? There's another
	// in HumanoidMobRenderer...
	HumanoidModel m_model;
};
