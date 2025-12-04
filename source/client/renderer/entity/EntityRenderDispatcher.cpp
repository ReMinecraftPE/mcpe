/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "EntityRenderDispatcher.hpp"
#include "client/app/Minecraft.hpp"
#include "renderer/ShaderConstants.hpp"
#include "../ItemInHandRenderer.hpp"

#include "client/model/models/PigModel.hpp"
#include "client/model/models/SheepModel.hpp"
#include "client/model/models/CowModel.hpp"
#include "client/model/models/ChickenModel.hpp"
#include "client/model/models/CreeperModel.hpp"
#include "client/model/models/SpiderModel.hpp"
#include "client/model/models/SkeletonModel.hpp"
#include "client/model/models/ZombieModel.hpp"

EntityRenderDispatcher* EntityRenderDispatcher::instance;
Vec3 EntityRenderDispatcher::off;

EntityRenderDispatcher::EntityRenderDispatcher()
	: m_HumanoidMobRenderer(new HumanoidModel(0.0f, 0.0f), 0.5f)
	, m_PigRenderer(new PigModel(0.0f), /*new PigModel(0.5f),*/ 0.7f)
	, m_SheepRenderer(new SheepModel(false), new SheepModel(true), 0.7f)
	, m_CowRenderer(new CowModel, 0.7f)
	, m_ChickenRenderer(new ChickenModel, 0.3f)
	, m_CreeperRenderer(new CreeperModel, 0.5f)
	, m_SpiderRenderer()
	, m_SkeletonRenderer(new SkeletonModel, 0.5f)
	, m_ZombieRenderer(new ZombieModel, 0.5f)
	, m_ArrowRenderer()
	, m_FallingTileRenderer()
{
	m_pItemInHandRenderer = nullptr;
	m_tileRenderer = new TileRenderer();
	m_pTextures = nullptr;
	m_pLevel = nullptr;
	m_pMinecraft = nullptr;
	m_pCamera = nullptr;
	m_rot = Vec2::ZERO;
	m_pOptions = nullptr;
	m_pFont = nullptr;

	// @TODO: Put these in an array or something
	m_HumanoidMobRenderer.init(this);
	m_PigRenderer.init(this);
	m_SheepRenderer.init(this);
	m_SpiderRenderer.init(this);
	m_SkeletonRenderer.init(this);
	m_CowRenderer.init(this);
	m_ChickenRenderer.init(this);
	m_CreeperRenderer.init(this);
	m_ZombieRenderer.init(this);
	m_ArrowRenderer.init(this);
	m_FallingTileRenderer.init(this);
	
	// TODO

	m_TntRenderer.init(this);
	m_CameraRenderer.init(this);
	m_ItemRenderer.init(this);
	m_RocketRenderer.init(this);
#ifdef ENH_ALLOW_SAND_GRAVITY
	m_FallingTileRenderer.init(this);
#endif
}

float EntityRenderDispatcher::distanceToSqr(const Vec3& pos)
{
	return pos.distanceToSqr(m_pos);
}

Font* EntityRenderDispatcher::getFont()
{
	return m_pFont;
}

EntityRenderDispatcher* EntityRenderDispatcher::getInstance()
{
	if(!instance)
		instance = new EntityRenderDispatcher;

	return instance;
}

EntityRenderer* EntityRenderDispatcher::getRenderer(Entity::RenderType renderType)
{
	// @HAL @TODO: make map
	switch (renderType)
	{
		case Entity::RENDER_TNT:
			return &m_TntRenderer;
		case Entity::RENDER_HUMANOID:
			return &m_HumanoidMobRenderer;
		case Entity::RENDER_ITEM:
			return &m_ItemRenderer;
		case Entity::RENDER_CAMERA:
			return &m_CameraRenderer;
		case Entity::RENDER_CHICKEN:
			return &m_ChickenRenderer;
		case Entity::RENDER_COW:
			return &m_CowRenderer;
		case Entity::RENDER_PIG:
			return &m_PigRenderer;
		case Entity::RENDER_SHEEP:
			return &m_SheepRenderer;
		case Entity::RENDER_SPIDER:
			return &m_SpiderRenderer;
		case Entity::RENDER_SKELETON:
			return &m_SkeletonRenderer;
		case Entity::RENDER_CREEPER:
			return &m_CreeperRenderer;
		case Entity::RENDER_ZOMBIE:
			return &m_ZombieRenderer;
		case Entity::RENDER_ARROW:
			return &m_ArrowRenderer;
		case Entity::RENDER_ROCKET:
			return &m_RocketRenderer;
#ifdef ENH_ALLOW_SAND_GRAVITY
		// TODO
		case Entity::RENDER_FALLING_TILE:
			return &m_FallingTileRenderer;
#endif
	}

	return nullptr;
}

EntityRenderer* EntityRenderDispatcher::getRenderer(const Entity& entity)
{
	Entity::RenderType renderType = entity.m_renderType;
	if (renderType == Entity::RENDER_DYNAMIC)
		renderType = entity.queryEntityRenderer();

	return getRenderer(renderType);
}

void EntityRenderDispatcher::onGraphicsReset()
{
	m_HumanoidMobRenderer.onGraphicsReset();
	m_PigRenderer.onGraphicsReset();
	m_SheepRenderer.onGraphicsReset();
	m_SpiderRenderer.onGraphicsReset();
	m_SkeletonRenderer.onGraphicsReset();
	m_CowRenderer.onGraphicsReset();
	m_ChickenRenderer.onGraphicsReset();
	m_CreeperRenderer.onGraphicsReset();
	m_ZombieRenderer.onGraphicsReset();
	m_ArrowRenderer.onGraphicsReset();
	
	m_TntRenderer.onGraphicsReset();
	m_CameraRenderer.onGraphicsReset();
	m_ItemRenderer.onGraphicsReset();
	m_RocketRenderer.onGraphicsReset();
#ifdef ENH_ALLOW_SAND_GRAVITY
	m_FallingTileRenderer.onGraphicsReset();
#endif
}

void EntityRenderDispatcher::prepare(Level* level, Textures* textures, Font* font, const Mob* camera, Options* options, float f)
{
	m_pLevel = level;
	m_pTextures = textures;
	m_pCamera = camera;
	m_pFont = font;
	m_pOptions = options;
	m_rot = camera->m_oRot    + (camera->m_rot - camera->m_oRot)    * f;
	m_pos = camera->m_posPrev + (camera->m_pos - camera->m_posPrev) * f;
}

void EntityRenderDispatcher::render(const Entity& entity, float a)
{
	Vec3 pos = Vec3(entity.m_posPrev + (entity.m_pos - entity.m_posPrev) * a);
	float yaw = entity.m_oRot.x + a * (entity.m_rot.x - entity.m_oRot.x);

	float bright = entity.getBrightness(1.0f);
	currentShaderColor = Color::WHITE;
	currentShaderDarkColor = Color(bright, bright, bright);

	render(entity, pos - off, yaw, a);
}

void EntityRenderDispatcher::render(const Entity& entity, const Vec3& pos, float rot, float a)
{
	EntityRenderer* pRenderer = getRenderer(entity);

	if (!pRenderer)
	{
		LOG_E("Failed to fetch renderer for entity: %s", entity.getDescriptor().getEntityType().getName().c_str());
		throw std::bad_cast();
	}

	pRenderer->render(entity, pos, rot, a);
	pRenderer->postRender(entity, pos, rot, a);
}

void EntityRenderDispatcher::setLevel(Level* level)
{
	m_pLevel = level;
}

void EntityRenderDispatcher::setMinecraft(Minecraft* mc)
{
	m_pMinecraft = mc;
}

void EntityRenderDispatcher::onAppSuspended()
{
	//m_tileRenderer->clearTileCache();
}
