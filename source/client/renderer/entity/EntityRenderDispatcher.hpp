/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <vector>
#include <map>

#include "client/app/AppPlatformListener.hpp"
#include "EntityRenderer.hpp"
#include "../TileRenderer.hpp"

class Minecraft;
class Font;
class Level;
class Entity;
class Textures;
class ItemInHandRenderer;

class EntityRenderDispatcher : public AppPlatformListener
{
protected:
	typedef std::vector<EntityRenderer*> EntityRenderers;
	typedef std::map<Entity::RenderType, EntityRenderer*> EntityRendererMap;

public:
	EntityRenderDispatcher();
	~EntityRenderDispatcher();

protected:
	void _addRenderer(Entity::RenderType renderType, EntityRenderer* pRenderer);

public:
	float distanceToSqr(const Vec3& pos);
	Font* getFont();
	EntityRenderer* getRenderer(const Entity& entity);
	EntityRenderer* getRenderer(Entity::RenderType renderType);
	void onGraphicsReset();
	void prepare(Level*, Textures*, Font*, const Mob* camera, Options*, float);
	void render(const Entity& entity, float);
	void render(const Entity& entity, const Vec3& pos, float rot, float a);
	void setLevel(Level*);
	void setMinecraft(Minecraft*);

	void onAppSuspended() override;

	static EntityRenderDispatcher* getInstance();

protected:
	// @TODO: replace with unordered_map
	EntityRenderers m_entityRenderers;
	EntityRendererMap m_entityRendererMap;

public:
	ItemInHandRenderer* m_pItemInHandRenderer;
	TileRenderer* m_tileRenderer;

	Textures* m_pTextures;
	Level* m_pLevel;
	Minecraft* m_pMinecraft;
	const Mob* m_pCamera;
	Vec2 m_rot;
	Options* m_pOptions;
	Vec3 m_pos;
	Font* m_pFont;

	static EntityRenderDispatcher* instance;
	static Vec3 off;
};

