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
#include "SynchedEntityData.hpp"
#include "EntityTypeDescriptor.hpp"

#define C_ENTITY_FLAG_ONFIRE (0)
#define C_ENTITY_FLAG_SNEAKING (1)
#define C_ENTITY_FLAG_RIDING (2)

class Level;
class Player;
class ItemInstance;
class ItemEntity;

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
	RENDER_ARROW,

	// custom
	RENDER_FALLING_TILE = 50,
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
protected:
	typedef int8_t SharedFlag;
public:
	typedef int32_t ID;
public:
	class EventType
	{
	public:
		typedef int8_t ID;
		enum
		{
			NONE,
			JUMP,
			HURT,
			DEATH,
			START_ATTACKING,
			STOP_ATTACKING
		};
	};

private:
	void _init();
public:
	Entity() { _init(); }
	Entity(Level*);
	virtual ~Entity();

protected:
	virtual bool getSharedFlag(SharedFlag flag) const;
	virtual void setSharedFlag(SharedFlag flag, bool value);

public:
	virtual void reset();
	virtual void setLevel(Level*);
	virtual void removed();
	virtual void setPos(const Vec3& pos);
	virtual void remove();
	virtual void move(const Vec3& posIn);
	virtual void moveTo(const Vec3& pos);
	virtual void moveTo(const Vec3& pos, const Vec2& rot);
	virtual void absMoveTo(const Vec3& pos);
	virtual void absMoveTo(const Vec3& pos, const Vec2& rot);
	virtual void moveRelative(const Vec3& pos);
	virtual void lerpTo(const Vec3& pos);
	virtual void lerpTo(const Vec3& pos, const Vec2& rot, int p = 3);
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
	virtual float getShadowHeightOffs() const { return m_bbHeight / 2.0f; }
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
	virtual bool isOnFire() const { return m_fireTicks > 0 || getSharedFlag(C_ENTITY_FLAG_ONFIRE); }
	virtual bool isRiding() const { return /*m_pRiding != nullptr ||*/ getSharedFlag(C_ENTITY_FLAG_RIDING); }
	virtual bool isSneaking() const { return getSharedFlag(C_ENTITY_FLAG_SNEAKING); }
	virtual void setSneaking(bool value) { setSharedFlag(C_ENTITY_FLAG_SNEAKING, value); }
	virtual bool isAlive() const { return m_bRemoved; }
	virtual bool isPlayer() const { return false; }
	virtual bool isMob() const { return false; }
	virtual bool interpolateOnly() const { return false; }
	virtual bool isCreativeModeAllowed() const { return false; }
	virtual bool shouldRender(Vec3& camPos) const;
	virtual bool shouldRenderAtSqrDistance(float distSqr) const;
	virtual bool hurt(Entity*, int);
	virtual void animateHurt();
	virtual float getPickRadius() const { return 0.1f; }
	virtual ItemEntity* spawnAtLocation(ItemInstance*, float);
	virtual ItemEntity* spawnAtLocation(int, int);
	virtual ItemEntity* spawnAtLocation(int, int, float);
	virtual void awardKillScore(Entity* pKilled, int score);
	virtual void setEquippedSlot(int, int, int);
	virtual void setRot(const Vec2& rot, bool rebound = false);
	virtual void setSize(float rad, float height);
	virtual void setPos(EntityPos*);
	virtual void resetPos(bool respawn = false);
	virtual void outOfWorld();
	virtual void checkFallDamage(float f, bool b);
	virtual void causeFallDamage(float f);
	virtual void markHurt();
	virtual void burn(int);
	virtual void lavaHurt();
	virtual int queryEntityRenderer();
	virtual const AABB* getCollideBox() const;
	virtual AABB* getCollideAgainstBox(Entity* ent) const;
	virtual void handleInsidePortal();
	virtual void handleEntityEvent(EventType::ID eventId);
	//virtual void thunderHit(LightningBolt*);
	void load(const CompoundTag& tag);
	bool save(CompoundTag& tag) const;
	void saveWithoutId(CompoundTag& tag) const;
	virtual void addAdditionalSaveData(CompoundTag& tag) const;
	virtual void readAdditionalSaveData(const CompoundTag& tag);
	// Removed by Mojang. See https://stackoverflow.com/questions/962132/why-is-a-call-to-a-virtual-member-function-in-the-constructor-a-non-virtual-call
	//virtual void defineSynchedData();
	EntityType::ID getEncodeId() const;
	Entity::ID hashCode() const { return m_EntityID; }

	const EntityTypeDescriptor& getDescriptor() const { return *m_pDescriptor; }
	SynchedEntityData& getEntityData() { return m_entityData; }
	const SynchedEntityData& getEntityData() const { return m_entityData; }

	bool operator==(const Entity& other) const;

	float distanceToSqr_inline(const Vec3& pos) const
	{
		return
			(m_pos.x - pos.x) * (m_pos.x - pos.x) +
			(m_pos.y - pos.y) * (m_pos.y - pos.y) +
			(m_pos.z - pos.z) * (m_pos.z - pos.z);
	}

protected:
	SynchedEntityData m_entityData;
	bool m_bMakeStepSound;
	const EntityTypeDescriptor* m_pDescriptor;

public:
	Vec3 m_pos;
	bool m_bInAChunk;
	ChunkPos m_chunkPos;
	int m_chunkPosY;
	int field_20; // unused Vec3?
	int field_24;
	int field_28;
	Entity::ID m_EntityID;
	float field_30;
	bool m_bBlocksBuilding;
	Level* m_pLevel;
	Vec3 m_oPos; // "o" in Java or "xo" "yo" "zo"
	Vec3 m_vel;
	Vec2 m_rot;
	Vec2 m_oRot; // "RotO" in Java or "xRotO" "yRotO"
	AABB m_hitbox;
	bool m_bOnGround;
	bool m_bHorizontalCollision;
	bool m_bCollision;
	bool m_bVerticalCollision;
	bool m_bHurt;
	bool m_bIsInWeb;
	uint8_t m_bSlide;
	bool m_bRemoved;
	bool m_bIsInvisible;
	bool m_bForceRemove;
	float m_heightOffset;
	float m_bbWidth;
	float m_bbHeight;
	float field_90;
	float m_walkDist;
	Vec3 m_posPrev;
	float m_ySlideOffset;
	float m_footSize;
	bool m_bNoPhysics;
	float m_pushthrough;
	int m_tickCount;
	int m_invulnerableTime;
	int m_airCapacity;
	int16_t m_fireTicks;
	int m_flameTime;
	int field_C8;  // @NOTE: Render type? (eEntityRenderType)
	float m_distanceFallen; // Supposed to be protected
	int16_t m_airSupply;
	bool m_bWasInWater;
	bool m_bFireImmune;
	bool m_bFirstTick;
	int m_nextStep;

public:
	static Entity::ID entityCounter;
	static Random sharedRandom;
};
