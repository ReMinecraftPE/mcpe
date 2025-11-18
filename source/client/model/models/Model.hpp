/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "client/model/geom/Cube.hpp"
#include "client/app/AppPlatformListener.hpp"
#include "renderer/MaterialPtr.hpp"

class Mob;
class ModelPart;

class Model : public AppPlatformListener
{
protected:
	class Materials
	{
	public:
		mce::MaterialPtr entity;
		mce::MaterialPtr entity_alphatest;
		mce::MaterialPtr entity_alphablend;
		mce::MaterialPtr entity_static;
		mce::MaterialPtr entity_emissive;
		mce::MaterialPtr entity_emissive_alpha;
		mce::MaterialPtr entity_change_color;
		mce::MaterialPtr entity_glint;
		mce::MaterialPtr entity_alphatest_glint;

		Materials();
	};

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
	bool m_bIsBaby;
	int m_textureWidth;
	int m_textureHeight;
	mce::MaterialPtr* m_pMaterial;
	Materials m_materials;
	std::vector<ModelPart*> m_parts;
};
