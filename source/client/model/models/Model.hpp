/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "client/model/geom/Cube.hpp"
#include "client/app/AppPlatformListener.hpp"

class Mob;
class ModelPart;

class Model : public AppPlatformListener
{
public:
	Model(int width, int height);
	virtual void clear();
	virtual void onGraphicsReset();
	virtual void prepareMobModel(Mob*, float, float, float);
	virtual void render(float, float, float, float, float, float);
	virtual void setupAnim(float, float, float, float, float, float);
	virtual void setBrightness(float);

	static void copyModelPart(const ModelPart& a, ModelPart& b);

public:
	float field_4;
	bool m_bRiding;
	int m_textureWidth;
	int m_textureHeight;
	std::vector<ModelPart*> m_parts;
	bool m_bIsBaby;
};
