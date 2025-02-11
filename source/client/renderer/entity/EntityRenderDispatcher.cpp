/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "EntityRenderDispatcher.hpp"
#include "client/app/Minecraft.hpp"
#include "../ItemInHandRenderer.hpp"

#include "client/model/PigModel.hpp"
#include "client/model/SheepModel.hpp"
#include "client/model/CowModel.hpp"
#include "client/model/ChickenModel.hpp"
#include "client/model/CreeperModel.hpp"

EntityRenderDispatcher* EntityRenderDispatcher::instance;
Vec3 EntityRenderDispatcher::off;

EntityRenderDispatcher::EntityRenderDispatcher() :
	m_HumanoidMobRenderer(new HumanoidModel(0.0f, 0.0f), 0.5f),
	m_PigRenderer(new PigModel(0.0f), /*new PigModel(0.5f),*/ 0.7f),
	m_SheepRenderer(new SheepModel(false), new SheepModel(true), 0.7f),
	m_CowRenderer(new CowModel, 0.7f),
	m_ChickenRenderer(new ChickenModel, 0.3f),
	m_CreeperRenderer(new CreeperModel, 0.5f)
{
	m_pItemInHandRenderer = nullptr;
	m_pTextures = nullptr;
	m_pLevel = nullptr;
	m_pMinecraft = nullptr;
	m_pMob = nullptr;
	m_rot = Vec2::ZERO;
	m_pOptions = nullptr;
	m_pFont = nullptr;

	m_HumanoidMobRenderer.init(this);
	m_PigRenderer.init(this);
	m_SheepRenderer.init(this);
	m_CowRenderer.init(this);
	m_ChickenRenderer.init(this);
	m_CreeperRenderer.init(this);
	
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

EntityRenderer* EntityRenderDispatcher::getRenderer(int renderType)
{
	switch (renderType)
	{
		case RENDER_TNT:
			return &m_TntRenderer;
		case RENDER_HUMANOID:
			return &m_HumanoidMobRenderer;
		case RENDER_ITEM:
			return &m_ItemRenderer;
		case RENDER_CAMERA:
			return &m_CameraRenderer;
		case RENDER_CHICKEN:
			return &m_ChickenRenderer;
		case RENDER_COW:
			return &m_CowRenderer;
		case RENDER_PIG:
			return &m_PigRenderer;
		case RENDER_SHEEP:
			return &m_SheepRenderer;
		case RENDER_CREEPER:
			return &m_CreeperRenderer;
		case RENDER_ROCKET:
			return &m_RocketRenderer;
#ifdef ENH_ALLOW_SAND_GRAVITY
		// TODO
		case RENDER_FALLING_TILE:
			return &m_FallingTileRenderer;
#endif
	}

	return nullptr;
}

EntityRenderer* EntityRenderDispatcher::getRenderer(Entity* pEnt)
{
	int renderType = pEnt->field_C8;
	if (renderType == RENDER_DYNAMIC)
		renderType = pEnt->queryEntityRenderer();

	return getRenderer(renderType);
}

void EntityRenderDispatcher::onGraphicsReset()
{
	m_HumanoidMobRenderer.onGraphicsReset();
}

void EntityRenderDispatcher::prepare(Level* level, Textures* textures, Font* font, Mob* mob, Options* options, float f)
{
	m_pLevel = level;
	m_pTextures = textures;
	m_pMob = mob;
	m_pFont = font;
	m_pOptions = options;
	m_rot = mob->m_rotPrev + (mob->m_rot - mob->m_rotPrev) * f;
	m_pos = mob->m_posPrev + (mob->m_pos - mob->m_posPrev) * f;
}

void EntityRenderDispatcher::render(Entity* entity, float a)
{
	Vec3 pos = Vec3(entity->m_posPrev + (entity->m_pos - entity->m_posPrev) * a);
	float yaw = entity->m_rotPrev.x + a * (entity->m_rot.x - entity->m_rotPrev.x);

	float bright = entity->getBrightness(1.0f);
	glColor4f(bright, bright, bright, 1.0f);

	render(entity, pos - off, yaw, a);
}

void EntityRenderDispatcher::render(Entity* entity, const Vec3& pos, float rot, float a)
{
	EntityRenderer* pRenderer = getRenderer(entity);
	if (pRenderer)
	{
#ifndef ORIGINAL_CODE
		if (pRenderer == &m_HumanoidMobRenderer)
			m_HumanoidMobRenderer.m_pHumanoidModel->m_bSneaking = entity->isSneaking();
		else
			m_HumanoidMobRenderer.m_pHumanoidModel->m_bSneaking = false;
#endif

		pRenderer->render(entity, pos.x, pos.y, pos.z, rot, a);
		pRenderer->postRender(entity, pos, rot, a);
	}
}

void EntityRenderDispatcher::setLevel(Level* level)
{
	m_pLevel = level;
}

void EntityRenderDispatcher::setMinecraft(Minecraft* mc)
{
	m_pMinecraft = mc;
}
