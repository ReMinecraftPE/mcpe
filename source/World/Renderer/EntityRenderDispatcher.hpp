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

class Minecraft;
class Font;
class Level;
class Entity;
class Textures;
class ItemInHandRenderer;

class EntityRenderDispatcher
{
public:
	EntityRenderDispatcher();
	float distanceToSqr(float x, float y, float z);
	Font* getFont();
	EntityRenderer* getRenderer(Entity* pEnt);
	void onGraphicsReset();
	void prepare(Level*, Textures*, Font*, Mob*, Options*, float);
	void render(Entity*, float);
	void render(Entity*, float, float, float, float, float);
	void setLevel(Level*);
	void setMinecraft(Minecraft*);

	static EntityRenderDispatcher* getInstance();

public:
	ItemInHandRenderer* m_pItemInHandRenderer = nullptr;
	HumanoidMobRenderer m_HumanoidMobRenderer;
	TntRenderer m_TntRenderer;
	ItemRenderer m_ItemRenderer;
	TripodCameraRenderer m_CameraRenderer;
	Textures* m_pTextures = nullptr;
	Level* m_pLevel = nullptr;
	Minecraft* m_pMinecraft = nullptr;
	Mob* m_pMob = nullptr;
	float m_yaw = 0.0f;
	float m_pitch = 0.0f;
	Options* m_pOptions = nullptr;
	Vec3 m_pos;
	Font* m_pFont = nullptr;

#if defined(ENH_ALLOW_SAND_GRAVITY)
	FallingTileRenderer m_FallingTileRenderer;
#endif

	static EntityRenderDispatcher* instance;
	static float xOff, yOff, zOff;
};

