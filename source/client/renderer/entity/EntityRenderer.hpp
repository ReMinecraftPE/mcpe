/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "client/renderer/Font.hpp"
#include "client/renderer/renderer/EntityShaderManager.hpp"
#include "world/phys/AABB.hpp"
#include "world/phys/Vec3.hpp"

class EntityRenderDispatcher;
class Level;
class Tile;

class EntityRenderer : public EntityShaderManager
{
protected:
	class Materials
	{
	public:
		mce::MaterialPtr entity;
		mce::MaterialPtr entity_alphatest;
		mce::MaterialPtr entity_alphatest_glint;
		mce::MaterialPtr name_tag;
		mce::MaterialPtr name_tag_depth_tested;
		mce::MaterialPtr name_text_depth_tested;

		Materials();
	};

private:
	static bool _areShadowsAvailable;
public:
	static bool areShadowsAvailable() { return _areShadowsAvailable; }
	static void setAreShadowsAvailable(bool value) { _areShadowsAvailable = value; }

private:
	Level* getLevel() const;
	void renderFlame(const Entity& entity, const Vec3& pos, float a);

public:
	EntityRenderer();
	bool bindTexture(const std::string& file, bool isRequired = true);
	Font* getFont();
	void init(EntityRenderDispatcher* d);
	static void render(const AABB&, const Vec3& pos);
	static void renderFlat(const AABB&);
	void postRender(const Entity& entity, const Vec3& pos, float rot, float a);

	virtual void render(const Entity& entity, const Vec3& pos, float rot, float a) = 0;
	virtual void onGraphicsReset();

protected:
	Materials m_materials;
public:
	float m_shadowRadius;
	float m_shadowStrength;
	EntityRenderDispatcher* m_pDispatcher;
};
