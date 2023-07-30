/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "AABB.hpp"
#include "Vec3.hpp"
#include "Material.hpp"
#include "Tile.hpp"
#include "ItemInstance.hpp"

class Level;
class Player;
class ItemInstance;

enum eEntityRenderType
{
	RENDER_NONE,
	RENDER_TNT,
	RENDER_HUMANOID,
	RENDER_ITEM,
	RENDER_CAMERA,

	// custom
	RENDER_FALLING_TILE = 50,
};

struct EntityPos
{
	Vec3 m_pos;
	float m_yaw = 0, m_pitch = 0;
	bool m_bHasRot = false, m_bHasPos = false;

	EntityPos() {};

	EntityPos(const Vec3& pos)
	{
		m_pos = pos;
		m_bHasPos = true;
	}

	EntityPos(float yaw, float pitch)
	{
		m_yaw = yaw;
		m_pitch = pitch;
		m_bHasRot = true;
	}

	EntityPos(const Vec3& pos, float yaw, float pitch)
	{
		m_pos = pos;
		m_yaw = yaw;
		m_pitch = pitch;
		m_bHasPos = true;
		m_bHasRot = true;
	}
};

class Entity
{
public:
	Entity(Level*);
	virtual ~Entity();
	virtual void setLevel(Level*);
	virtual void removed();
	virtual void setPos(float x, float y, float z);
	virtual void remove();
	virtual int move(float x, float y, float z);
	virtual void moveTo(float x, float y, float z, float yaw, float pitch);
	virtual void absMoveTo(float x, float y, float z, float yaw, float pitch);
	virtual void moveRelative(float x, float z, float y);
	virtual void lerpTo(float x, float y, float z, float yaw, float pitch, int i);
	virtual void lerpMotion(float x, float y, float z);
	virtual void turn(float yaw, float pitch);
	virtual void interpolateTurn(float yaw, float pitch);
	virtual void tick();
	virtual void baseTick();
	virtual bool intersects(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
	virtual bool isFree(float offX, float offY, float offZ);
	virtual bool isFree(float offX, float offY, float offZ, float expand);
	virtual bool isInWall();
	virtual bool isInWater();
	virtual bool isInLava();
	virtual bool isUnderLiquid(Material*);
	virtual float getHeadHeight();
	virtual float getShadowHeightOffs();
	virtual float getBrightness(float f);
	virtual float distanceTo(Entity*);
	virtual float distanceToSqr(float x, float y, float z);
	virtual float distanceTo(float x, float y, float z);
	virtual float distanceToSqr(Entity*);
	virtual int interactPreventDefault();
	virtual bool interact(Player*);
	virtual void playerTouch(Player*);
	virtual void push(Entity*);
	virtual void push(float x, float y, float z);
	virtual bool isPickable();
	virtual bool isPushable();
	virtual bool isShootable();
	virtual bool isSneaking();
	virtual bool isAlive();
	virtual bool isOnFire();
	virtual bool isPlayer();
	virtual bool isCreativeModeAllowed();
	virtual bool shouldRender(Vec3& camPos);
	virtual bool shouldRenderAtSqrDistance(float distSqr);
	virtual bool hurt(Entity*, int);
	virtual void animateHurt();
	virtual float getPickRadius();
	// virtual void spawnAtLocation(ItemInstance*, float);
	// virtual void spawnAtLocation(int, int);
	// virtual void spawnAtLocation(int, int, float);
	virtual void awardKillScore(Entity* pKilled, int score);
	virtual void setEquippedSlot(int, int, int);
	virtual void setRot(float yaw, float pitch);
	virtual void setSize(float rad, float height);
	virtual void setPos(EntityPos*);
	virtual void resetPos();
	virtual void outOfWorld();
	virtual void checkFallDamage(float f, bool b);
	virtual void causeFallDamage(float f);
	virtual void markHurt();
	virtual void burn(int);
	virtual void lavaHurt();

	int hashCode();

	bool operator==(const Entity& other) const;

	float distanceToSqr_inline(float x, float y, float z) const
	{
		return
			(m_pos.x - x) * (m_pos.x - x) +
			(m_pos.y - y) * (m_pos.y - y) +
			(m_pos.z - z) * (m_pos.z - z);
	}

public:
	static int entityCounter;
	static Random sharedRandom;

	Vec3 m_pos;
	bool m_bInAChunk = false;
	int m_chunkX = 0;
	int m_chunkY = 0;
	int m_chunkZ = 0;
	int field_20 = 0;
	int field_24 = 0;
	int field_28 = 0;
	int m_EntityID;
	float field_30 = 1.0f;
	uint8_t field_34 = 0;
	Level* m_pLevel = nullptr;
	Vec3 field_3C;
	Vec3 m_vel;
	float m_yaw   = 0.0f;
	float m_pitch = 0.0f;
	//maybe these are the actual m_yaw and m_pitch, and
	//the one I annotated are the destination yaw and pitch.
	//interpolateTurn doesn't modify them, so I highly suspect
	//this to be the case.
	float field_5C = 0.0f;
	float field_60 = 0.0f;
	AABB m_hitbox;
	bool field_7C = false;
	bool field_7D = false;
	bool field_7E = false;
	bool field_7F = false;
	bool m_bHurt = false;
	uint8_t field_81 = 1;
	bool m_bRemoved = false;
	float field_84 = 0.0f;
	float field_88 = 0.6f;
	float field_8C = 1.8f;
	float field_90 = 0.0f;
	float field_94 = 0.0f;
	Vec3 field_98;
	float field_A4 = 0.0f;
	float field_A8 = 0.0f;
	bool m_bNoCollision = false;
	float field_B0 = 0.0f;
	int field_B4 = 0;
	int field_B8 = 0;
	int field_BC = 300;
	int field_C0 = 0;
	int field_C4;
	int field_C8 = 0;  // @NOTE: Render type? (eEntityRenderType)
	float m_distanceFallen = 0.0f;
	int field_D0 = 300;
	uint8_t field_D4 = 0;
	bool field_D5 = false;
	bool field_D6 = true;
	int field_D8 = 1;
};
