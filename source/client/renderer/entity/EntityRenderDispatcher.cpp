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
#include "client/model/CowModel.hpp"
#include "client/model/ChickenModel.hpp"
#include "client/model/CreeperModel.hpp"

EntityRenderDispatcher* EntityRenderDispatcher::instance;
float EntityRenderDispatcher::xOff, EntityRenderDispatcher::yOff, EntityRenderDispatcher::zOff;

EntityRenderDispatcher::EntityRenderDispatcher() :
	m_HumanoidMobRenderer(new HumanoidModel(0.0f, 0.0f), 0.0f),
	m_PigRenderer(new PigModel(0.0f), 0.0f),
	m_CowRenderer(new CowModel, 0.0f),
	m_ChickenRenderer(new ChickenModel, 0.0f),
	m_CreeperRenderer(new CreeperModel, 0.5f)
{
	m_pItemInHandRenderer = nullptr;
	m_pTextures = nullptr;
	m_pLevel = nullptr;
	m_pMinecraft = nullptr;
	m_pMob = nullptr;
	m_yaw = 0.0f;
	m_pitch = 0.0f;
	m_pOptions = nullptr;
	m_pFont = nullptr;

	m_HumanoidMobRenderer.init(this);
	m_PigRenderer.init(this);
	m_CowRenderer.init(this);
	m_ChickenRenderer.init(this);
	m_CreeperRenderer.init(this);
	
	// TODO

	m_TntRenderer.init(this);
	m_CameraRenderer.init(this);
	m_ItemRenderer.init(this);
	m_RocketRenderer.init(this);
	m_FallingTileRenderer.init(this);
}

float EntityRenderDispatcher::distanceToSqr(float x, float y, float z)
{
	float dX = x - m_pos.x;
	float dY = y - m_pos.y;
	float dZ = z - m_pos.z;

	return dX * dX + dY * dY + dZ * dZ;
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
		case RENDER_CREEPER:
			return &m_CreeperRenderer;
		case RENDER_ROCKET:
			return &m_RocketRenderer;
		// TODO
		case RENDER_FALLING_TILE:
			return &m_FallingTileRenderer;
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
	m_yaw   = mob->field_5C + f * (mob->m_yaw   - mob->field_5C);
	m_pitch = mob->field_60 + f * (mob->m_pitch - mob->field_60);
	m_pos.x = mob->field_98.x + f * (mob->m_pos.x - mob->field_98.x);
	m_pos.y = mob->field_98.y + f * (mob->m_pos.x - mob->field_98.y);
	m_pos.z = mob->field_98.z + f * (mob->m_pos.x - mob->field_98.z);
}

void EntityRenderDispatcher::render(Entity* entity, float f)
{
	float x = entity->field_98.x + f * (entity->m_pos.x - entity->field_98.x);
	float y = entity->field_98.y + f * (entity->m_pos.y - entity->field_98.y);
	float z = entity->field_98.z + f * (entity->m_pos.z - entity->field_98.z);
	float yaw = entity->field_5C + f * (entity->m_yaw - entity->field_5C);

	float bright = entity->getBrightness(1.0f);
	glColor4f(bright, bright, bright, 1.0f);

	render(entity, x - xOff, y - yOff, z - zOff, yaw, f);
}

void EntityRenderDispatcher::render(Entity* entity, float a, float b, float c, float d, float e)
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

		pRenderer->render(entity, a, b, c, d, e);
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
