/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "world/phys/AABB.hpp"
#include "world/phys/Vec3.hpp"
#include "client/model/HumanoidModel.hpp"
#include "client/renderer/Font.hpp"

class EntityRenderDispatcher;
class Level;
class Tile;

class EntityRenderer
{
private:
	static bool _areShadowsAvailable;
public:
	static bool areShadowsAvailable() { return _areShadowsAvailable; }
	static void setAreShadowsAvailable(bool value) { _areShadowsAvailable = value; }

private:
	Level* getLevel() const;
	void renderFlame(Entity* e, const Vec3& pos, float a);
	void renderShadow(Entity* e, const Vec3& pos, float pow, float a);
	void renderTileShadow(Tile* tt, const Vec3& pos, TilePos& tilePos, float pow, float r, const Vec3& oPos);

public:
	EntityRenderer();
	bool bindTexture(const std::string& file, bool isRequired = true);
	Font* getFont();
	void init(EntityRenderDispatcher* d);
	static void render(const AABB&, const Vec3& pos);
	static void renderFlat(const AABB&);
	void postRender(Entity* entity, const Vec3& pos, float rot, float a);

	virtual void render(Entity* entity, const Vec3& pos, float rot, float a) = 0;
	virtual void onGraphicsReset();

public:
	float m_shadowRadius;
	float m_shadowStrength;
	EntityRenderDispatcher* m_pDispatcher;

	// @HUH: Why is there a HumanoidModel here? There's another
	// in HumanoidMobRenderer...
	HumanoidModel m_model;
};
