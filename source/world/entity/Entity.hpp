/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "world/phys/Vec3.hpp"
#include "world/phys/Vec2.hpp"
#include "world/phys/AABB.hpp"
#include "world/level/Material.hpp"
#include "world/level/levelgen/chunk/ChunkPos.hpp"
#include "world/tile/Tile.hpp"
#include "world/item/ItemInstance.hpp"
#include "EntityType.hpp"
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

enum eCreatureBaseType
{
	BASE_NONE,
	BASE_MONSTER,
	BASE_ANIMAL,
};

struct EntityPos
{
	Vec3 m_pos;
	Vec2 m_rot;
	bool m_bHasRot, m_bHasPos;

	EntityPos()
		: m_pos(Vec3::ZERO), m_rot(Vec2::ZERO)
	{
        m_bHasRot = false; m_bHasPos = false;
	};

	EntityPos(const Vec3& pos)
		: m_pos(pos), m_rot(Vec2::ZERO)
	{
		m_bHasPos = true; m_bHasRot = false;
	}

	EntityPos(const Vec2& rot)
		: m_pos(Vec3::ZERO), m_rot(rot)
	{
		m_bHasPos = false; m_bHasRot = true;
	}

	EntityPos(const Vec3& pos, const Vec2& rot)
		: m_pos(pos), m_rot(rot)
	{
		m_bHasPos = true; m_bHasRot = true;
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
	virtual void setPos(const Vec3& pos);
	virtual void remove();
	virtual int move(const Vec3& pos);
	virtual void moveTo(const Vec3& pos, const Vec2& rot = Vec2::ZERO);
	virtual void absMoveTo(const Vec3& pos, const Vec2& rot = Vec2::ZERO);
	virtual void moveRelative(const Vec3& pos);
	virtual void lerpTo(const Vec3& pos);
	virtual void lerpTo(const Vec3& pos, const Vec2& rot, int i);
	virtual void lerpMotion(const Vec3& pos);
	virtual void turn(const Vec2& rot);
	virtual void interpolateTurn(const Vec2& rot);
	virtual void tick();
	virtual void baseTick();
	virtual bool intersects(const Vec3& min, const Vec3& max) const;
	virtual bool isFree(const Vec3& off) const;
	virtual bool isFree(const Vec3& off, float expand) const;
	virtual bool isInWall() const;
	virtual bool isInWater();
	virtual bool isInLava() const;
	virtual bool isUnderLiquid(Material*) const;
	virtual float getHeadHeight() const { return 0.0f; }
	virtual float getShadowHeightOffs() const { return field_8C / 2.0f; }
	virtual float getBrightness(float f) const;
	virtual float distanceTo(Entity*) const;
	virtual float distanceToSqr(const Vec3& pos) const;
	virtual float distanceTo(const Vec3& pos) const;
	virtual float distanceToSqr(Entity*) const;
	virtual int interactPreventDefault();
	virtual bool interact(Player*);
	virtual void playerTouch(Player*);
	virtual void push(Entity*);
	virtual void push(const Vec3& pos);
	virtual bool isPickable() const { return false; }
	virtual bool isPushable() const { return false; }
	virtual bool isShootable() const { return false; }
	virtual bool isSneaking() const { return false; }
	virtual bool isAlive() const { return m_bRemoved; }
	virtual bool isOnFire() const { return m_fireTicks > 0; }
	virtual bool isPlayer() const { return false; }
	virtual bool isCreativeModeAllowed() const { return false; }
	virtual bool shouldRender(Vec3& camPos) const;
	virtual bool shouldRenderAtSqrDistance(float distSqr) const;
	virtual bool hurt(Entity*, int);
	virtual void animateHurt();
	virtual float getPickRadius() const { return 0.1f; }
	virtual void spawnAtLocation(ItemInstance*, float);
	virtual void spawnAtLocation(int, int);
	virtual void spawnAtLocation(int, int, float);
	virtual void awardKillScore(Entity* pKilled, int score);
	virtual void setEquippedSlot(int, int, int);
	virtual void setRot(const Vec2& rot);
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
	virtual int getCreatureBaseType() const { return BASE_NONE; }
	virtual int getEntityTypeId() const { return ENTITY_TYPE_NONE; }

	int hashCode() const { return m_EntityID; }

	bool operator==(const Entity& other) const;

	float distanceToSqr_inline(const Vec3& pos) const
	{
		return
			(m_pos.x - pos.x) * (m_pos.x - pos.x) +
			(m_pos.y - pos.y) * (m_pos.y - pos.y) +
			(m_pos.z - pos.z) * (m_pos.z - pos.z);
	}

public:
	static int entityCounter;
	static Random sharedRandom;

	Vec3 m_pos;
	bool m_bInAChunk;
	ChunkPos m_chunkPos;
	int m_chunkPosY;
	int field_20;
	int field_24;
	int field_28;
	int m_EntityID;
	float field_30;
	uint8_t field_34;
	Level* m_pLevel;
	Vec3 m_ySlideOffset; // "o" in Java or "xo" ""yo" "zo"
	Vec3 m_vel;
	Vec2 m_rot;
	//maybe these are the actual m_yaw and m_pitch, and
	//the one I annotated are the destination yaw and pitch.
	//interpolateTurn doesn't modify them, so I highly suspect
	//this to be the case.
	Vec2 m_rotPrev;
	AABB m_hitbox;
	bool m_onGround;
	bool field_7D;
	bool field_7E;
	bool field_7F;
	bool m_bHurt;
	uint8_t field_81;
	bool m_bRemoved;
	float m_heightOffset;
	float field_88;
	float field_8C;
	float field_90;
	float field_94;
	Vec3 m_posPrev;
	float field_A4;
	float field_A8;
	bool m_bNoCollision;
	float field_B0;
	int field_B4;
	int field_B8;
	int field_BC;
	int m_fireTicks;
	int m_flameTime;
	int field_C8;  // @NOTE: Render type? (eEntityRenderType)
	float m_distanceFallen;
	int field_D0;
	uint8_t field_D4;
	bool field_D5;
	bool field_D6;
	int field_D8;
};
