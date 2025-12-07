/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "EntityRenderer.hpp"
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

class Minecraft;
class Font;
class Level;
class Entity;
class Textures;
class ItemInHandRenderer;

class EntityRenderDispatcher : AppPlatformListener
{
public:
	EntityRenderDispatcher();
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

public:
	ItemInHandRenderer* m_pItemInHandRenderer;
	TileRenderer* m_tileRenderer;

	HumanoidMobRenderer m_HumanoidMobRenderer;
	PigRenderer m_PigRenderer;
	SheepRenderer m_SheepRenderer;
	CowRenderer m_CowRenderer;
	ChickenRenderer m_ChickenRenderer;
	TntRenderer m_TntRenderer;
	//padding??
	ItemRenderer m_ItemRenderer;
	CreeperRenderer m_CreeperRenderer;
	SpiderRenderer m_SpiderRenderer;
	HumanoidMobRenderer m_SkeletonRenderer;
	HumanoidMobRenderer m_ZombieRenderer;
	//SheepRenderer m_SheepRenderer;
	//SheepFurRenderer m_SheepFurRenderer;
	TripodCameraRenderer m_CameraRenderer;
	ArrowRenderer m_ArrowRenderer;
#ifdef ENH_ALLOW_SAND_GRAVITY
	FallingTileRenderer m_FallingTileRenderer;
#endif
	RocketRenderer m_RocketRenderer;

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

