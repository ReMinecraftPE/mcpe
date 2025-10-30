/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Entity.hpp"

#include "common/Logger.hpp"
#include "nbt/CompoundTag.hpp"
#include "world/level/Level.hpp"

#include "Player.hpp"

#define TOTAL_AIR_SUPPLY (300)
#define DATA_SHARED_FLAGS_ID (0)

int Entity::entityCounter;
Random Entity::sharedRandom;

void Entity::_init()
{
	m_bInAChunk = false;
	field_20 = 0;
	field_24 = 0;
	field_28 = 0;
	field_30 = 1.0f;
    m_bBlocksBuilding = false;
	m_pLevel = nullptr;
	m_bOnGround = false;
	m_bHorizontalCollision = false;
	m_bCollision = false;
	m_bVerticalCollision = false;
	m_bHurt = false;
	m_bIsInWeb = false;
	m_bSlide = 1;
	m_bRemoved = false;
	m_bIsInvisible = false;
	m_bForceRemove = false;
	m_heightOffset = 0.0f;
	m_bbWidth = 0.6f;
	m_bbHeight = 1.8f;
	field_90 = 0.0f;
	m_walkDist = 0.0f;
	m_bMakeStepSound = true;
	m_ySlideOffset = 0.0f;
	m_footSize = 0.0f;
	m_bNoPhysics = false;
	m_pushthrough = 0.0f;
    m_tickCount = 0;
	m_invulnerableTime = 0;
	m_airCapacity = TOTAL_AIR_SUPPLY;
	m_fireTicks = 0;
	m_flameTime = 1;
    m_tickCount = 0;
	field_C8 = 0;  // @NOTE: Render type? (eEntityRenderType)
	m_distanceFallen = 0.0f;
	m_airSupply = TOTAL_AIR_SUPPLY;
	m_bWasInWater = false;
	m_bFireImmune = false;
	m_bFirstTick = true;
	m_nextStep = 1;
	m_pDescriptor = &EntityTypeDescriptor::unknown;
}

Entity::Entity(Level* pLevel)
{
	_init();

	m_pLevel = pLevel;
	m_EntityID = ++entityCounter;
	setPos(Vec3::ZERO);

	m_entityData.define<SharedFlag>(DATA_SHARED_FLAGS_ID, 0);
}

Entity::~Entity()
{
	m_entityData.clear();
}

bool Entity::getSharedFlag(SharedFlag flag) const
{
	return (getEntityData().get<int8_t>(DATA_SHARED_FLAGS_ID) & 1 << flag) != 0;
}

void Entity::setSharedFlag(SharedFlag flag, bool value)
{
	SynchedEntityData& entityData = getEntityData();
	int8_t var3 = entityData.get<int8_t>(DATA_SHARED_FLAGS_ID);
	if (value)
	{
		entityData.set(DATA_SHARED_FLAGS_ID, (int8_t)(var3 | 1 << flag));
	}
	else
	{
		entityData.set(DATA_SHARED_FLAGS_ID, (int8_t)(var3 & ~(1 << flag)));
	}
}

void Entity::setLevel(Level* pLvl)
{
	m_pLevel = pLvl;
}

void Entity::removed()
{
	m_bRemoved = true;
}

void Entity::setPos(const Vec3& pos)
{
	m_pos = pos;

	float halfSize = m_bbWidth / 2;
	float lowY = m_pos.y - m_heightOffset + m_ySlideOffset;

	m_hitbox = AABB(
		m_pos.x - halfSize,
		lowY,
		m_pos.z - halfSize,
		m_pos.x + halfSize,
		lowY + m_bbHeight,
		m_pos.z + halfSize);
}

void Entity::remove()
{
	m_bRemoved = true;
}

void Entity::move(const Vec3& pos)
{
	if (m_bNoPhysics)
	{
		m_hitbox.move(pos);
		m_pos.x = (m_hitbox.min.x + m_hitbox.max.x) / 2.0f;
		m_pos.y = m_hitbox.min.y + m_heightOffset - m_ySlideOffset;
		m_pos.z = (m_hitbox.min.z + m_hitbox.max.z) / 2.0f;
	}
	else
	{
		m_ySlideOffset *= 0.4f;
		Vec3 newPos(pos);
		if (m_bIsInWeb)
		{
			m_bIsInWeb = false;
			newPos.x *= 0.25f;
			newPos.y *= 0.05f;
			newPos.z *= 0.25f;
			m_vel = Vec3::ZERO;
			m_distanceFallen = 0.0f;
		}
		float aPosX = m_pos.x;
		float aPosZ = m_pos.z;
		float cPosX = newPos.x;
		float cPosY = newPos.y;
		float cPosZ = newPos.z;
		AABB lastHit = m_hitbox;
		bool validSneaking = m_bOnGround && isSneaking();
		if (validSneaking)
		{
			for (float dx = 0.05f; pos.x != 0.0f && m_pLevel->getCubes(this, AABB(m_hitbox).move(newPos.x, -1.0f, 0.0f))->size() == 0; cPosX = newPos.x)
			{
				if (newPos.x < dx && newPos.x >= -dx)
					newPos.x = 0.0;
				else if (newPos.x > 0.0f)
					newPos.x -= dx;
				else
					newPos.x += dx;
			}

			for (float dz = 0.05f; newPos.z != 0.0f && m_pLevel->getCubes(this, AABB(m_hitbox).move(0.0f, -1.0f, newPos.z))->size() == 0; cPosZ = newPos.z)
			{
				if (newPos.z < dz && newPos.z >= -dz)
					newPos.z = 0.0f;
				else if (newPos.z > 0.0f)
					newPos.z -= dz;
				else
					newPos.z += dz;
			}
		}

		AABBVector* cubes = m_pLevel->getCubes(this, AABB(m_hitbox).expand(newPos.x, newPos.y, newPos.z));

		for (int i = 0; i < int(cubes->size()); ++i)
			newPos.y = cubes->at(i).clipYCollide(m_hitbox, newPos.y);

		m_hitbox.move(0.0f, newPos.y, 0.0f);
		if (!m_bSlide && cPosY != newPos.y)
			newPos = Vec3::ZERO;

		bool lastsOnGround = m_bOnGround || cPosY != newPos.y && cPosY < 0.0;

		for (int i = 0; i < int(cubes->size()); ++i)
			newPos.x = cubes->at(i).clipXCollide(m_hitbox, newPos.x);
	
		m_hitbox.move(newPos.x, 0.0f, 0.0f);
		if (!m_bSlide && cPosX != newPos.x)
			newPos = Vec3::ZERO;

		for (int i = 0; i < int(cubes->size()); ++i)
			newPos.z = cubes->at(i).clipZCollide(m_hitbox, newPos.z);

		m_hitbox.move(0.0f, 0.0f, newPos.z);
		if (!m_bSlide && cPosZ != newPos.z)
			newPos = Vec3::ZERO;

		if (m_footSize > 0.0f && lastsOnGround && m_ySlideOffset < 0.05F && (cPosX != newPos.x || cPosZ != newPos.z))
		{
			Vec3 oldPos = newPos;
			newPos.x = cPosX;
			newPos.y = m_footSize;
			newPos.z = cPosZ;
			AABB oldHit = m_hitbox;
			m_hitbox = lastHit;
			cubes = m_pLevel->getCubes(this, AABB(m_hitbox).expand(cPosX, newPos.y, cPosZ));

			for (int i = 0; i < int(cubes->size()); ++i)
				newPos.y = cubes->at(i).clipYCollide(m_hitbox, newPos.y);

			m_hitbox.move(0.0f, newPos.y, 0.0f);
			if (!m_bSlide && cPosY != newPos.y)
				newPos = Vec3::ZERO;

			for (int i = 0; i < int(cubes->size()); ++i)
				newPos.x = cubes->at(i).clipXCollide(m_hitbox, newPos.x);

			m_hitbox.move(newPos.x, 0.0f, 0.0f);
			if (!m_bSlide && cPosX != newPos.x)
				newPos = Vec3::ZERO;

			for (int i = 0; i < int(cubes->size()); ++i)
				newPos.z = cubes->at(i).clipZCollide(m_hitbox, newPos.z);

			m_hitbox.move(0.0f, 0.0f, newPos.z);
			if (!m_bSlide && cPosZ != newPos.z)
				newPos = Vec3::ZERO;

			if (oldPos.x * oldPos.x + oldPos.z * oldPos.z >= newPos.x * newPos.x + newPos.z * newPos.z)
			{
				newPos = oldPos;
				m_hitbox = oldHit;
			}
			else
			{
				float hitYOff = m_hitbox.min.y - int(m_hitbox.min.y);
				if (hitYOff > 0.0f)
					m_ySlideOffset = m_ySlideOffset + hitYOff + 0.01f;
			}
		}

		m_pos.x = (m_hitbox.min.x + m_hitbox.max.x) / 2.0f;
		m_pos.y = m_hitbox.min.y + m_heightOffset - m_ySlideOffset;
		m_pos.z = (m_hitbox.min.z + m_hitbox.max.z) / 2.0f;
		m_bHorizontalCollision = cPosX != newPos.x || cPosZ != newPos.z;
		m_bVerticalCollision = cPosY != newPos.y;
		m_bOnGround = cPosY != newPos.y && cPosY < 0.0f;
		m_bCollision = m_bHorizontalCollision || m_bVerticalCollision;
		checkFallDamage(newPos.y, m_bOnGround);
		if (cPosX != newPos.x)
			m_vel.x = 0.0f;

		if (cPosY != newPos.y)
			m_vel.y = 0.0f;

		if (cPosZ != newPos.z)
			m_vel.z = 0.0f;

		float diffX = m_pos.x - aPosX;
		float diffZ = m_pos.z - aPosZ;
		if (m_bMakeStepSound && !validSneaking)
		{
			m_walkDist = float(m_walkDist + Mth::sqrt(diffX * diffX + diffZ * diffZ) * 0.6f);
			TilePos tp(m_pos.x, m_pos.y - 0.2f - m_heightOffset, m_pos.z);
			TileID i = m_pLevel->getTile(tp);

			if (m_pLevel->getTile(tp.below()) == Tile::fence->m_ID)
				i = Tile::fence->m_ID;

			if (m_walkDist > m_nextStep && i > 0)
			{
				m_nextStep = m_walkDist + 1;

				const Tile::SoundType* sound = nullptr;
				// vol is * 0.15f in Java, is quiet for whatever reason, so bumping to 0.20f
				if (m_pLevel->getTile(tp.above()) == Tile::topSnow->m_ID)
				{
					sound = Tile::topSnow->m_pSound;
				}
				else if (!Tile::tiles[i]->m_pMaterial->isLiquid())
				{
					sound = Tile::tiles[i]->m_pSound;
				}

				if (sound != nullptr)
					m_pLevel->playSound(this, "step." + sound->m_name, sound->volume * 0.20f, sound->pitch);

				Tile::tiles[i]->stepOn(m_pLevel, tp, this);
			}
		}

		TilePos minPos(m_hitbox.min + 0.001f);
		TilePos maxPos(m_hitbox.max - 0.001f);
		TilePos tilePos;

		if (m_pLevel->hasChunksAt(minPos, TilePos(maxPos)))
		{
			for (tilePos.x = minPos.x; tilePos.x <= maxPos.x; tilePos.x++)
				for (tilePos.y = minPos.y; tilePos.y <= maxPos.y; tilePos.y++)
					for (tilePos.z = minPos.z; tilePos.z <= maxPos.z; tilePos.z++)
					{
						TileID tileID = m_pLevel->getTile(tilePos);
						if (tileID > 0)
							Tile::tiles[tileID]->entityInside(m_pLevel, tilePos, this);
					}
		}

		bool bIsInWater = isInWater();

		if (m_pLevel->containsFireTile(AABB(minPos, maxPos)))
		{
			burn(1);

			if (!bIsInWater)
			{
				m_fireTicks++;
				if (m_fireTicks == 0)
					m_fireTicks = 300;
			}
		}
		else if (m_fireTicks <= 0)
		{
			m_fireTicks = -m_flameTime;
		}

		if (bIsInWater && m_fireTicks > 0)
		{
			m_pLevel->playSound(this, "random.fizz", 0.7f, 1.6f + (sharedRandom.nextFloat() - sharedRandom.nextFloat()) * 0.4f);
			m_fireTicks = -m_flameTime;
		}

	}
}

void Entity::moveTo(const Vec3& pos)
{
	Vec3 newPos(pos);
	newPos.y += m_heightOffset;

	setPos(newPos);
	m_oPos = newPos;
	m_posPrev = newPos;
}

void Entity::moveTo(const Vec3& pos, const Vec2& rot)
{
	moveTo(pos);
	m_rot = rot;
}

void Entity::absMoveTo(const Vec3& pos)
{
	m_ySlideOffset = 0.0f;

	setPos(pos);
	m_oPos = pos;
}

void Entity::absMoveTo(const Vec3& pos, const Vec2& rot)
{
	absMoveTo(pos);

	m_oRot = rot;
	setRot(rot);

	// This looks like a rebounding check for the angle
	float dyRot = (m_oRot.y - m_rot.y);
	if (dyRot < -180.0f)
		m_oRot.y += 360.0f;
	if (dyRot >= 180.0f)
		m_oRot.y -= 360.0f;
}

void Entity::moveRelative(const Vec3& pos)
{
	float d = Mth::sqrt(pos.x * pos.x + pos.z * pos.z);
	if (d < 0.01f) return;
	if (d < 1.0f)
		d = 1.0f;

	Vec3 vel(pos);

	vel.y /= d;
	vel.x *= vel.y;
	vel.z *= vel.y;

	float yaw = m_rot.x * float(M_PI);
	float syaw = sinf(yaw / 180.0f);
	float cyaw = cosf(yaw / 180.0f);

	m_vel.x += vel.x * cyaw - vel.z * syaw;
	m_vel.z += vel.x * syaw + vel.z * cyaw;
}

void Entity::lerpTo(const Vec3& pos)
{
	setPos(pos);
}

void Entity::lerpTo(const Vec3& pos, const Vec2& rot, int p)
{
	lerpTo(pos);
	setRot(rot);
}

void Entity::lerpMotion(const Vec3& pos)
{
	m_vel = pos;
}

void Entity::turn(const Vec2& rot)
{
	if (rot == Vec2::ZERO) return;

	Vec2 rotOld = m_rot;

	interpolateTurn(rot);

	m_oRot.x += m_rot.x - rotOld.x;
	m_oRot.y += m_rot.y - rotOld.y;
}

void Entity::reset()
{
	// TODO is this it
	m_posPrev = m_oPos = m_pos;
	m_oRot = m_rot;
	m_bRemoved = false;
	m_distanceFallen = 0.0f;
	m_bFireImmune = false;
	m_fireTicks = 0;
}

void Entity::interpolateTurn(const Vec2& rot)
{
	setRot(Vec2(
		m_rot.x + rot.x * 0.15f,
		m_rot.y - rot.y * 0.15f
	));

	// can't rotate more than facing fully up or fully down
	if (m_rot.y < -90.0f)
		m_rot.y = -90.0f;
	if (m_rot.y > 90.0f)
		m_rot.y = 90.0f;
}

void Entity::tick()
{
	baseTick();
}

void Entity::baseTick()
{
	//@TODO: untangle the gotos

	field_90 = m_walkDist;
	m_oPos = m_pos;
    m_tickCount++;
	m_oRot = m_rot;
	if (isInWater())
	{
		if (!m_bWasInWater && !m_bFirstTick)
		{
			float dist = Mth::sqrt(m_vel.y * m_vel.y + m_vel.x * m_vel.x * 0.2f + m_vel.z * m_vel.z * 0.2f) * 0.2f;
			if (dist > 1.0f)
				dist = 1.0f;

			m_pLevel->playSound(this, "random.splash", dist, 1.0f + 0.4f * (sharedRandom.nextFloat() - sharedRandom.nextFloat()));

			float f1 = floorf(m_hitbox.min.y);

			for (int i = 0; float(i) < m_bbWidth * 20.0f + 1.0f; i++)
			{
				m_pLevel->addParticle(
					"bubble",
					Vec3(
						m_pos.x + m_bbWidth * (sharedRandom.nextFloat() * 2.0f - 1.0f),
						f1 + 1.0f,
						m_pos.z + m_bbWidth * (sharedRandom.nextFloat() * 2.0f - 1.0f)
					),
					Vec3(
						m_vel.x,
						m_vel.y - 0.2f * sharedRandom.nextFloat(),
						m_vel.z
					)
				);
			}

			for (int i = 0; float(i) < m_bbWidth * 20.0f + 1.0f; i++)
			{
				m_pLevel->addParticle(
					"splash",
					Vec3(
						m_pos.x + m_bbWidth * (sharedRandom.nextFloat() * 2.0f - 1.0f),
						f1 + 1.0f,
						m_pos.z + m_bbWidth * (sharedRandom.nextFloat() * 2.0f - 1.0f)
					),
					Vec3(
						m_vel.x,
						m_vel.y - 0.2f * sharedRandom.nextFloat(),
						m_vel.z
					)
				);
			}
		}

		m_bWasInWater = true;
		m_fireTicks = 0;
		m_distanceFallen = 0;

		if (m_pLevel->m_bIsClientSide)
			goto label_4;
	}
	else
	{
		m_bWasInWater = false;

		if (m_pLevel->m_bIsClientSide)
		{
		label_4:
			m_fireTicks = 0;
			goto label_5;
		}
	}

	if (m_fireTicks <= 0)
	{
	label_5:
		if (!isInLava())
			goto label_6;
		goto label_15;
	}

	if (m_bFireImmune)
	{
		m_fireTicks -= 4;
		if (m_fireTicks < 0)
			m_fireTicks = 0;
		goto label_5;
	}

	if (m_fireTicks % 20 == 0)
	{
		hurt(nullptr, 1);
	}

	m_fireTicks--;

	if (isInLava())
	{
	label_15:
		lavaHurt();
	}

label_6:
	if (m_pos.y < -64.0f)
		outOfWorld();

	m_bFirstTick = false;
}

bool Entity::intersects(const Vec3& min, const Vec3& max) const
{
	AABB aabb(min, max);
	return aabb.intersect(m_hitbox);
}

bool Entity::isFree(const Vec3& off) const
{
	AABB aabb = m_hitbox;
	aabb.move(off);

	AABBVector* pCubes = m_pLevel->getCubes(this, aabb);
	if (!pCubes)
		return false;

	return !m_pLevel->containsAnyLiquid(aabb);
}

bool Entity::isFree(const Vec3& off, float expand) const
{
	AABB aabb = m_hitbox;
	aabb.move(off);
	aabb.grow(expand, expand, expand);

	AABBVector* pCubes = m_pLevel->getCubes(this, aabb);
	if (!pCubes)
		return false;

	return !m_pLevel->containsAnyLiquid(aabb);
}

bool Entity::isInWall() const
{
	return m_pLevel->isSolidTile(TilePos(m_pos.x, m_pos.y + getHeadHeight(), m_pos.z));
}

bool Entity::isInWater()
{
	AABB aabb = m_hitbox;
	aabb.grow(0, -0.4f, 0);
	return m_pLevel->checkAndHandleWater(aabb, Material::water, this);
}

bool Entity::isInLava() const
{
	AABB aabb = m_hitbox;
	aabb.grow(-0.1f, -0.4f, -0.1f);
	return m_pLevel->containsMaterial(aabb, Material::lava);
}

bool Entity::isUnderLiquid(Material* pMtl) const
{
	TilePos tilePos(m_pos.x, m_pos.y + getHeadHeight(), m_pos.z);

	Tile* pTile = Tile::tiles[m_pLevel->getTile(tilePos)];
	if (!pTile || pTile->m_pMaterial != pMtl)
		return false;

	TileData data = m_pLevel->getData(tilePos);
	int level = data <= 7 ? data + 1 : 1;

	return float(tilePos.y) < float(tilePos.y + 1) - (float(level) / 9.0f - 0.11111f);
}

float Entity::getBrightness(float f) const
{
	TilePos tilePos(m_pos.x, m_pos.y - m_heightOffset + (m_hitbox.max.y - m_hitbox.min.y) * 0.66f, m_pos.z);

	TilePos tileMin(m_hitbox.min);
	TilePos tileMax(m_hitbox.max);

	if (!m_pLevel->hasChunksAt(tileMin, tileMax))
		return 0;

	return m_pLevel->getBrightness(tilePos);
}

float Entity::distanceTo(Entity* pEnt) const
{
	return distanceTo(pEnt->m_pos);
}

float Entity::distanceTo(const Vec3& pos) const
{
	return m_pos.distanceTo(pos);
}

float Entity::distanceToSqr(Entity* pEnt) const
{
	return distanceToSqr(pEnt->m_pos);
}

float Entity::distanceToSqr(const Vec3& pos) const
{
	return m_pos.distanceToSqr(pos);
}

int Entity::interactPreventDefault()
{
	return 0;
}

bool Entity::interact(Player* player)
{
	return false;
}

void Entity::playerTouch(Player* player)
{

}

void Entity::push(Entity* bud)
{
	float diffX = bud->m_pos.x - m_pos.x;
	float diffZ = bud->m_pos.z - m_pos.z;
	float maxDiff = Mth::absMax(diffX, diffZ);

	if (maxDiff < 0.01f) return;

	float x1 = Mth::sqrt(maxDiff);
	float x2 = 1.0f / x1;
	if (x2 > 1.0f)
		x2 = 1.0f;
	float x3 = 1.0f - m_pushthrough;
	float x4 = x3 * diffX / x1 * x2 * 0.05f;
	float x5 = x3 * diffZ / x1 * x2 * 0.05f;

	push(Vec3( -x4, 0.0f, -x5));
	bud->push(Vec3(x4, 0.0f, x5));
}

void Entity::push(const Vec3& pos)
{
	m_vel += pos;
}

bool Entity::shouldRender(Vec3& camPos) const
{
	if (m_bIsInvisible)
		return false;

	return shouldRenderAtSqrDistance(distanceToSqr(camPos));
}

bool Entity::shouldRenderAtSqrDistance(float distSqr) const
{
	float maxDist = (field_30 * 64.0f) * (((m_hitbox.max.z - m_hitbox.min.z) + (m_hitbox.max.x - m_hitbox.min.x) + (m_hitbox.max.y - m_hitbox.min.y)) / 3.0f);

	return maxDist * maxDist > distSqr;
}

bool Entity::hurt(Entity* pAttacker, int damage)
{
	markHurt();
	return 0;
}

void Entity::animateHurt()
{

}

ItemEntity* Entity::spawnAtLocation(ItemInstance* itemInstance, float y)
{
	ItemEntity *itemEntity = new ItemEntity(m_pLevel, Vec3(m_pos.x, m_pos.y + y, m_pos.z), itemInstance);
	// @TODO: not sure what this does, or is for
	itemEntity->m_oPos.x = 10;
	m_pLevel->addEntity(itemEntity);
	
	return itemEntity;
}

ItemEntity* Entity::spawnAtLocation(int itemID, int amount)
{
	return spawnAtLocation(itemID, amount, 0);
}

ItemEntity* Entity::spawnAtLocation(int itemID, int amount, float y)
{
	ItemInstance* itemInstance = new ItemInstance(itemID, amount, 0);
	return spawnAtLocation(itemInstance, y);
}

void Entity::awardKillScore(Entity* pKilled, int score)
{

}

void Entity::setEquippedSlot(int a, int b, int c)
{

}

void Entity::setRot(const Vec2& rot, bool rebound)
{
	if (rebound)
	{
		m_rot.x = Mth::abs(rot.x) > 360.0f ? fmod(rot.x, 360.0f) : rot.x;
		m_rot.y = Mth::abs(rot.y) > 360.0f ? fmod(rot.y, 360.0f) : rot.y;
	}
	else
	{
		m_rot = rot;
	}
}

void Entity::setSize(float rad, float height)
{
	m_bbWidth = rad;
	m_bbHeight = height;
}

void Entity::setPos(EntityPos* pPos)
{
	if (pPos->m_bHasPos)
		setPos(pPos->m_pos);
	else
		setPos(m_pos);

	if (pPos->m_bHasRot)
		setRot(pPos->m_rot);
	else
		setRot(m_rot);
}

void Entity::resetPos(bool respawn)
{
	if (!m_pLevel)
		// No level?  Fine
		return;

	if (m_pos.y <= 0.0f)
		// It's in the void.  It's probably not inside of any tiles.
		return;

	// Push the entity up while it's inside solid tiles.
	while (true)
	{
		setPos(m_pos);

		AABBVector* pCubes = m_pLevel->getCubes(this, m_hitbox);

		// if we aren't inside any tiles, great!
		if (!pCubes->size())
			break;

		m_pos.y += 1.0f;
	}

	m_vel = Vec3::ZERO;
	m_rot.y = 0.0f;
}

void Entity::outOfWorld()
{
	remove();
}

void Entity::checkFallDamage(float fDeltaY, bool bHitGround)
{
	if (bHitGround)
	{
		if (m_distanceFallen > 0.0f)
		{
			causeFallDamage(m_distanceFallen);
			m_distanceFallen = 0.0f;
		}
	}
	else if (fDeltaY < 0.0f)
	{
		m_distanceFallen -= fDeltaY;
	}
}

void Entity::causeFallDamage(float f)
{
	// stub
}

void Entity::markHurt()
{
	m_bHurt = true;
}

void Entity::burn(int x)
{
	if (!m_bFireImmune)
		hurt(nullptr, 4);
}

void Entity::lavaHurt()
{
	if (!m_bFireImmune)
	{
		hurt(nullptr, 4);
		m_fireTicks = 600;
	}
}

int Entity::queryEntityRenderer()
{
	// If field_C8 is equal to RENDER_DYNAMIC, EntityRenderDispatcher
	// calls here. Used for sheared sheep.
	return 0;
}

const AABB* Entity::getCollideBox() const
{
	return nullptr;
}

AABB* Entity::getCollideAgainstBox(Entity* ent) const
{
	return nullptr;
}

void Entity::handleInsidePortal()
{
}

void Entity::handleEntityEvent(EventType::ID eventId)
{
	LOG_W("Unknown EntityEvent ID: %d, EntityType: %s", eventId, getDescriptor().getEntityType().getName().c_str());
}

/*void Entity::thunderHit(LightningBolt* bolt)
{
	burn(5);
	++m_fireTicks;
	if (m_fireTicks == 0)
		m_fireTicks = 300;

}*/

void Entity::load(const CompoundTag& tag)
{
	const ListTag* posTag = tag.getList("Pos");
	const ListTag* motionTag = tag.getList("Motion");
	const ListTag* rotTag = tag.getList("Rotation");
	m_vel.x = motionTag->getFloat(0);
	m_vel.y = motionTag->getFloat(1);
	m_vel.z = motionTag->getFloat(2);
	if (Mth::abs(m_vel.x) > 10.0f)
	{
		m_vel.x = 0.0f;
	}
	if (Mth::abs(m_vel.y) > 10.0f)
	{
		m_vel.y = 0.0f;
	}
	if (Mth::abs(m_vel.z) > 10.0f)
	{
		m_vel.z = 0.0f;
	}
	m_posPrev.x = m_oPos.x = m_pos.x = posTag->getFloat(0);
	m_posPrev.y = m_oPos.y = m_pos.y = posTag->getFloat(1);
	m_posPrev.z = m_oPos.z = m_pos.z = posTag->getFloat(2);
	m_oRot.y = m_rot.y = rotTag->getFloat(0);
	m_oRot.x = m_rot.x = rotTag->getFloat(1);
	m_distanceFallen = tag.getFloat("FallDistance");
	m_fireTicks = tag.getInt16("Fire");
	m_airSupply = tag.getInt16("Air");
	m_bOnGround = tag.getBoolean("OnGround");
	setPos(m_pos);
	setRot(m_rot);
	readAdditionalSaveData(tag);
}

bool Entity::save(CompoundTag& tag) const
{
	EntityType::ID id = getEncodeId();
	if (m_bRemoved)
		return false;
	if (id == EntityType::UNKNOWN)
	{
		LOG_W("Failed to save unknown entity!");
		return false;
	}

	tag.putInt32("id", id);
	saveWithoutId(tag);
	return true;
}

void Entity::saveWithoutId(CompoundTag& tag) const
{
	tag.put("Pos", ListTagFloatAdder(m_pos.x)(m_pos.y + m_ySlideOffset)(m_pos.z));
	tag.put("Motion", ListTagFloatAdder(m_vel.x)(m_vel.y)(m_vel.z));
	tag.put("Rotation", ListTagFloatAdder(m_rot.y)(m_rot.x));
	tag.putFloat("FallDistance", m_distanceFallen);
	tag.putInt16("Fire", m_fireTicks);
	tag.putInt16("Air", m_airSupply);
	tag.putBoolean("OnGround", m_bOnGround);
	//tag.putInt32("PortalCooldown", m_portalCooldown);
	//tag.putBoolean("IsGlobal", m_bIsGlobal);

	/*if (isRide())
		tag.put("LinksTag", saveLinks());*/

	addAdditionalSaveData(tag);
}

void Entity::addAdditionalSaveData(CompoundTag& tag) const
{
}

void Entity::readAdditionalSaveData(const CompoundTag& tag)
{
}

EntityType::ID Entity::getEncodeId() const
{
	return getDescriptor().getEntityType().getId();
}

bool Entity::operator==(const Entity& other) const
{
	return m_EntityID == other.m_EntityID;
}
