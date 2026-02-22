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

#include "HumanoidMobRenderer.hpp"
#include "TripodCameraRenderer.hpp"
#include "TntRenderer.hpp"
#include "ItemRenderer.hpp"
#include "FallingTileRenderer.hpp"
#include "PigRenderer.hpp"
#include "SheepRenderer.hpp"
#include "CowRenderer.hpp"
#include "ChickenRenderer.hpp"
#include "CreeperRenderer.hpp"
#include "SpiderRenderer.hpp"
#include "ArrowRenderer.hpp"
#include "RocketRenderer.hpp"

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

	_addRenderer(Entity::RENDER_HUMANOID,     new HumanoidMobRenderer(new HumanoidModel, 0.5f));
	_addRenderer(Entity::RENDER_PIG,          new PigRenderer(        new PigModel(0.0f), /*new PigModel(0.5f),*/ 0.7f));
	_addRenderer(Entity::RENDER_SHEEP,        new SheepRenderer(      new SheepModel(false), new SheepModel(true), 0.7f));
	_addRenderer(Entity::RENDER_COW,          new CowRenderer(        new CowModel,      0.7f));
	_addRenderer(Entity::RENDER_CHICKEN,      new ChickenRenderer(    new ChickenModel,  0.3f));
	_addRenderer(Entity::RENDER_CREEPER,      new CreeperRenderer(    new CreeperModel,  0.5f));
	_addRenderer(Entity::RENDER_SPIDER,       new SpiderRenderer());
	_addRenderer(Entity::RENDER_SKELETON,     new HumanoidMobRenderer(new SkeletonModel, 0.5f));
	_addRenderer(Entity::RENDER_ZOMBIE,       new HumanoidMobRenderer(new ZombieModel,   0.5f));
	_addRenderer(Entity::RENDER_ARROW,        new ArrowRenderer());
#ifdef ENH_ALLOW_SAND_GRAVITY
	_addRenderer(Entity::RENDER_FALLING_TILE, new FallingTileRenderer());
#endif
	_addRenderer(Entity::RENDER_TNT,          new TntRenderer());
	_addRenderer(Entity::RENDER_CAMERA,       new TripodCameraRenderer());
	_addRenderer(Entity::RENDER_ITEM,         new ItemRenderer());
	_addRenderer(Entity::RENDER_ROCKET,       new RocketRenderer());
}

EntityRenderDispatcher::~EntityRenderDispatcher()
{
	for (size_t i = 0; i < m_entityRenderers.size(); i++)
	{
		delete m_entityRenderers[i];
	}
}

void EntityRenderDispatcher::_addRenderer(Entity::RenderType renderType, EntityRenderer* pRenderer)
{
	pRenderer->init(this);

	m_entityRenderers.push_back(pRenderer);
	m_entityRendererMap[renderType] = pRenderer;
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
	return m_entityRendererMap[renderType];
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
	for (size_t i = 0; i < m_entityRenderers.size(); i++)
	{
		m_entityRenderers[i]->onGraphicsReset();
	}
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
