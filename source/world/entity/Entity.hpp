/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "world/phys/Vec3.hpp"
#include "world/phys/AABB.hpp"
#include "world/level/Material.hpp"
#include "world/tile/Tile.hpp"
#include "world/item/ItemInstance.hpp"
#include "common/Utils.hpp"

class Level;
class Player;
class ItemInstance;

enum eEntityRenderType
{
	RENDER_NONE,
	RENDER_DYNAMIC,
	RENDER_TNT,
	RENDER_HUMANOID,
	RENDER_ITEM,
	RENDER_CAMERA,
	RENDER_CHICKEN,
	RENDER_COW,
	RENDER_PIG,
	RENDER_SHEEP,
	RENDER_SHEEP_FUR,
	RENDER_ZOMBIE,
	RENDER_SKELETON,
	RENDER_SPIDER,
	RENDER_CREEPER,
	RENDER_ROCKET,

	// custom
	RENDER_FALLING_TILE = 50,
};

enum eEntityType
{
	TYPE_UNSAVED,
	
	TYPE_CHICKEN = 10,
	TYPE_COW,
	TYPE_PIG,
	TYPE_SHEEP,

	TYPE_ZOMBIE = 32,

	TYPE_ITEM = 64,
};

enum eCreatureBaseType
{
	BASE_NONE,
	BASE_MONSTER,
	BASE_ANIMAL,
};

struct EntityPos
{
	Vec3 m_pos;
	float m_yaw, m_pitch;
	bool m_bHasRot, m_bHasPos;

	EntityPos()
	{
        m_yaw = 0; m_pitch = 0;
        m_bHasRot = false; m_bHasPos = false;
	};

	EntityPos(const Vec3& pos)
	{
		m_pos = pos;
        m_yaw = 0; m_pitch = 0;
		m_bHasPos = true;
		m_bHasRot = false;
	}

	EntityPos(float yaw, float pitch)
	{
		m_yaw = yaw;
		m_pitch = pitch;
		m_bHasPos = false;
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
private:
	void _init();
public:
	Entity() { _init(); }
	Entity(Level*);
	virtual ~Entity();
	virtual void reset();
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
	virtual bool isMob();
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
	virtual int queryEntityRenderer();
	virtual int getCreatureBaseType();
	virtual int getEntityTypeId();

	virtual bool isLocalPlayer();

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
	bool m_bInAChunk;
	int m_chunkX;
	int m_chunkY;
	int m_chunkZ;
	int field_20;
	int field_24;
	int field_28;
	int m_EntityID;
	float field_30;
	uint8_t field_34;
	Level* m_pLevel;
	Vec3 field_3C;
	Vec3 m_vel;
	float m_yaw;
	float m_pitch;
	//maybe these are the actual m_yaw and m_pitch, and
	//the one I annotated are the destination yaw and pitch.
	//interpolateTurn doesn't modify them, so I highly suspect
	//this to be the case.
	float field_5C;
	float field_60;
	AABB m_hitbox;
	bool field_7C;
	bool field_7D;
	bool field_7E;
	bool field_7F;
	bool m_bHurt;
	uint8_t field_81;
	bool m_bRemoved;
	float field_84;
	float field_88;
	float field_8C;
	float field_90;
	float field_94;
	Vec3 field_98;
	float field_A4;
	float field_A8;
	bool m_bNoCollision;
	float field_B0;
	int field_B4;
	int field_B8;
	int field_BC;
	int field_C0;
	int field_C4;
	int field_C8;  // @NOTE: Render type? (eEntityRenderType)
	float m_distanceFallen;
	int field_D0;
	uint8_t field_D4;
	bool field_D5;
	bool field_D6;
	int field_D8;
};
