/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Entity.hpp"
#include "Player.hpp"
#include "world/level/Level.hpp"

int Entity::entityCounter;
Random Entity::sharedRandom;

void Entity::_init()
{
	m_bInAChunk = false;
	m_chunkPos = ChunkPos(0, 0);
	field_20 = 0;
	field_24 = 0;
	field_28 = 0;
	field_30 = 1.0f;
	field_34 = 0;
	m_pLevel = nullptr;
	m_rot = Vec2::ZERO;
	m_rotPrev = Vec2::ZERO;
	m_onGround = false;
	field_7D = false;
	field_7E = false;
	field_7F = false;
	m_bHurt = false;
	field_81 = 1;
	m_bRemoved = false;
	m_heightOffset = 0.0f;
	field_88 = 0.6f;
	field_8C = 1.8f;
	field_90 = 0.0f;
	field_94 = 0.0f;
	field_A4 = 0.0f;
	field_A8 = 0.0f;
	m_bNoCollision = false;
	field_B0 = 0.0f;
	field_B4 = 0;
	field_B8 = 0;
	field_BC = 300;
	m_fireTicks = 0;
	m_flameTime = 0;
	field_C8 = 0;  // @NOTE: Render type? (eEntityRenderType)
	m_distanceFallen = 0.0f;
	m_airSupply = 300;
	field_D4 = 0;
	field_D5 = false;
	field_D6 = true;
	field_D8 = 1;
	m_entityData = SynchedEntityData();
}

Entity::Entity(Level* pLevel)
{
	_init();

	m_pLevel = pLevel;
	m_EntityID = ++entityCounter;
	setPos(Vec3::ZERO);

	m_entityData.define<int8_t>(0, 0);
}

Entity::~Entity()
{
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

	float halfSize = field_88 / 2;
	float lowY = m_pos.y - m_heightOffset + field_A4;

	m_hitbox = AABB(
		m_pos.x - halfSize,
		lowY,
		m_pos.z - halfSize,
		m_pos.x + halfSize,
		lowY + field_8C,
		m_pos.z + halfSize);
}

void Entity::remove()
{
	m_bRemoved = true;
}

int Entity::move(const Vec3& pos)
{
	float x_1 = pos.x, z_1 = pos.z;

	if (m_bNoCollision)
	{
		// just move it. Don't perform any kind of collision
		m_hitbox.min += pos;
		m_hitbox.max += pos;

		m_pos.x = (m_hitbox.max.x + m_hitbox.min.x) / 2;
		m_pos.z = (m_hitbox.max.z + m_hitbox.min.z) / 2;
		m_pos.y = m_hitbox.min.y + m_heightOffset - field_A4;

		return 1300;
	}

	//@TODO: untangle the control flow

	float x1, x2, x3, x4, x5, x6;
	float x7, x8, x9, x10, x11, x12, x20;
	float x_2, z_2, x_3, z_3;
	float oldX, oldZ;
	bool b1, b2, b3, b4, b5, b6;
	AABB hitold;

    oldX = m_pos.x; oldZ = m_pos.z;

	x7 = m_hitbox.max.z;
	x8 = m_hitbox.max.y;
	x9 = m_hitbox.max.x;
	x10 = m_hitbox.min.z;
	x11 = m_hitbox.min.y;
	x12 = m_hitbox.min.x;

	x1 = m_hitbox.max.z;
	x2 = m_hitbox.max.y;
	x3 = m_hitbox.max.x;
	x4 = m_hitbox.min.z;
	x5 = m_hitbox.min.y;
	x6 = m_hitbox.min.x;

	if (!m_onGround)
	{
	label_4:

		z_2 = z_1;
		b1 = x_1 < 0.0f;
		b2 = x_1 > 0.0f;
		x_2 = x_1;
		b3 = z_1 < 0.0f;
		b4 = z_1 > 0.0f;
		b5 = false;
		goto label_5;
	}

	if (!isSneaking())
		goto label_4;

	if (x_1 == 0.0f)
	{
		x_2 = x_1;
		b1 = x_1 < 0.0f;
		b2 = x_1 > 0.0f;
	}
	else
	{
		x_2 = x_1;
		do
		{
			AABB aabb = m_hitbox;
			aabb.move(x_1, -1.0f, 0);

			AABBVector* cubes = m_pLevel->getCubes(this, aabb);

			if (cubes->size())
				break;

			if (x_1 < 0.05f && x_1 >= -0.05f)
			{
				x_2 = 0.0f;
				x_1 = 0.0f;
				break;
			}

			// @BUG: See the z_1 part
			if (x_1 <= 0.0f)
				x_1 = x_1 + 0.05f;
			else
				x_1 = x_1 - 0.05f;

			x_2 = x_1;
		} while (x_1 != 0.0f);

		b1 = x_1 < 0.0f;
		b2 = x_1 > 0.0f;
	}

	if (z_1 == 0.0f)
	{
		z_2 = z_1;
		b3 = z_1 < 0.0f;
		b4 = z_1 > 0.0f;
	}
	else
	{
		z_2 = z_1;
		do
		{
			AABB aabb = m_hitbox;
			aabb.move(0, -1.0f, z_1);

			AABBVector* cubes = m_pLevel->getCubes(this, aabb);

			if (cubes->size())
				break;

			if (z_1 < 0.05f && z_1 >= -0.05f)
			{
				z_2 = 0.0f;
				z_1 = 0.0f;
				break;
			}

			//@BUG: wouldn't this loop forever? Since if z_1 == 0.025f, it'd oscillate between -0.025f and +0.025f...
			if (z_1 <= 0.0f)
				z_1 = z_1 + 0.05f;
			else
				z_1 = z_1 - 0.05f;
		} while (z_1 != 0.0f);
		b3 = z_1 < 0.0f;
		b4 = z_1 > 0.0f;
	}

	b5 = true;

label_5:
	if (b1) x6 += x_1;
	if (b2) x3 += x_1;
	if (pos.y < 0.0f) x5 += pos.y;
	if (pos.y > 0.0f) x2 += pos.y;
	if (b3) x4 += pos.z;
	if (b4) x1 += pos.z;

	AABB scanAABB(x6, x5, x4, x3, x2, x1);
	AABBVector* pCubes = m_pLevel->getCubes(this, scanAABB);

	float newY = pos.y;
	for (int i = 0; i < pCubes->size(); i++)
	{
		const AABB& aabb = pCubes->at(i);
		newY = aabb.clipYCollide(m_hitbox, newY);
	}

	m_hitbox.move(0, newY, 0);

	if (!field_81 && newY != pos.y)
	{
		z_1 = 0.0f;
		x_1 = 0.0f;
		newY = 0.0f;
	}

	if (m_onGround)
	{
		b6 = true;
	}
	else
	{
		b6 = pos.y < 0.0f;
		if (newY == pos.y)
			b6 = 0;
	}

	for (int i = 0; i < pCubes->size(); i++)
	{
		const AABB& aabb = pCubes->at(i);
		x_1 = aabb.clipXCollide(m_hitbox, x_1);
	}

	m_hitbox.move(x_1, 0, 0);

	if (!field_81 && x_1 != x_2)
	{
		z_1 = 0.0f;
		x_1 = 0.0f;
		newY = 0.0f;
	}

	for (int i = 0; i < pCubes->size(); i++)
	{
		const AABB& aabb = pCubes->at(i);
		z_1 = aabb.clipZCollide(m_hitbox, z_1);
	}

	m_hitbox.move(0, 0, z_1);

	if (!field_81 && z_1 != z_2)
	{
		z_1 = 0.0f;
		x_1 = 0.0f;
		newY = 0.0f;
	}

	x20 = field_A8;
	if (x20 <= 0.0f || !b6)
	{
	label_44:
		z_3 = z_1;
		x_3 = x_1;
		goto label_45;
	}

    if (field_A4 >= 0.05f || (x_2 == x_1 && z_2 == z_1))
		goto label_44;

	// oh come on, undoing all our work??
	hitold = m_hitbox;
	m_hitbox = AABB(x12, x11, x10, x9, x8, x7);

	if (b1) x12 += x_2;
	if (b2) x9 += x_2;
	x8 += x20; //@BUG: missing if (x20 > 0) check?
	if (x20 < 0.0f) x11 += x20;
	if (b3) x10 += z_2;
	if (b4) x7 += z_2;

	{
		AABB scanAABB2(x12, x11, x10, x9, x8, x7);

		//@BUG: useless copy
		//@BUG: this doesn't actually copy anything
		*pCubes = *m_pLevel->getCubes(this, scanAABB2);
	}

	for (int i = 0; i < pCubes->size(); i++)
	{
		const AABB& aabb = pCubes->at(i);
		x20 = aabb.clipYCollide(m_hitbox, x20);
	}

	m_hitbox.move(0, x20, 0);

	if (field_81 || x20 == pos.y)
	{
		z_3 = z_2;
		x_3 = x_2;
	}
	else
	{
		x20 = 0.0f;
		z_3 = 0.0f;
		x_3 = 0.0f;
	}

	for (int i = 0; i < pCubes->size(); i++)
	{
		const AABB& aabb = pCubes->at(i);
		x_3 = aabb.clipXCollide(m_hitbox, x_3);
	}

	m_hitbox.move(x_3, 0, 0);

	if (!field_81 && x_2 != x_3)
	{
		x20 = 0.0f;
		z_3 = 0.0f;
		x_3 = 0.0f;
	}

	for (int i = 0; i < pCubes->size(); i++)
	{
		const AABB& aabb = pCubes->at(i);
		z_3 = aabb.clipZCollide(m_hitbox, z_3);
	}

	m_hitbox.move(0, 0, z_3);

	if (!field_81 && z_2 != z_3)
	{
		x20 = 0.0f;
		z_3 = 0.0f;
		x_3 = 0.0f;
	}

	// if we moved more this time than before?? no clue wtf this is about...
	if (z_1 * z_1 + x_1 * x_1 < z_3 * z_3 + x_3 * x_3)
	{
		field_A4 += 0.5f;
		newY = x20;
	}
	else
	{
		// don't get the rationale behind this at all...
		m_hitbox = hitold;
		z_3 = z_1;
		x_3 = x_1;
	}

label_45:
	m_pos.x = (m_hitbox.min.x + m_hitbox.max.x) / 2;
	m_pos.z = (m_hitbox.min.z + m_hitbox.max.z) / 2;
	m_pos.y = m_hitbox.min.y - field_A4 + m_heightOffset;

	field_7D = x_2 != x_3 || z_2 != z_3;
	field_7F = field_7D || newY != pos.y;
	m_onGround = pos.y < 0.0f && newY != pos.y;
	field_7E = newY != pos.y;

	checkFallDamage(newY, m_onGround);

	if (x_2 != x_3) m_vel.x = 0.0;
	if (newY != pos.y)  m_vel.y = 0.0;
	if (z_2 != z_3) m_vel.z = 0.0;

	if (!b5)
	{
		float diffX = (m_pos.x - oldX), diffZ = (m_pos.z - oldZ);
		field_94 += Mth::sqrt(diffZ * diffZ + diffX * diffX) * 0.6f;

		TilePos tilePos(Mth::floor(m_pos.x),
						Mth::floor(m_pos.y - 0.2f - m_heightOffset),
						Mth::floor(m_pos.z));

		TileID tileID = m_pLevel->getTile(tilePos);

		if (tileID > 0 && field_94 > float(field_D8))
		{
			++field_D8;
			bool bPlaySound = true;

			const Tile::SoundType *sound = Tile::tiles[tileID]->m_pSound;
			if (!isPlayer()) // no idea why this wasn't already a thing
				bPlaySound = false;
			else if (m_pLevel->getTile(tilePos.above()) == Tile::topSnow->m_ID)
				sound = Tile::topSnow->m_pSound;
			else if (Tile::tiles[tileID]->m_pMaterial->isLiquid())
				bPlaySound = false;

			if (bPlaySound)
				m_pLevel->playSound(this, "step." + sound->m_name, sound->volume * 0.15f, sound->pitch);

			Tile::tiles[tileID]->stepOn(m_pLevel, tilePos, this);
		}
	}

	// Check whether the entity is inside of any tiles.

	TilePos minPos(m_hitbox.min);
	TilePos maxPos(m_hitbox.max);
	TilePos tilePos = TilePos();

	if (m_pLevel->hasChunksAt(TilePos(m_hitbox.min), TilePos(m_hitbox.max)))
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

	field_A4 *= 0.4f;

	bool bIsInWater = isInWater();

	if (m_pLevel->containsFireTile(m_hitbox))
	{
		burn(true);

		if (bIsInWater)
		{
		label_76:
			if (m_fireTicks > 0)
				m_fireTicks = -m_flameTime;

			return 1300;
		}

		if (m_fireTicks == 0)
			m_fireTicks = 300;
		else
			m_fireTicks++;
	}
	else
	{
		if (m_fireTicks <= 0)
			m_fireTicks = -m_flameTime;

		if (bIsInWater)
			goto label_76;
	}

	return 1300;
}

void Entity::moveTo(const Vec3& pos, const Vec2& rot)
{
	Vec3 newPos(pos);
	newPos.y += m_heightOffset;

	setPos(newPos);
	m_ySlideOffset = newPos;
	m_posPrev = newPos;

	m_rot = rot;
}

void Entity::absMoveTo(const Vec3& pos, const Vec2& rot)
{
	field_A4 = 0.0f;

	m_rotPrev = rot;
	setRot(rot);

	setPos(pos);
	m_ySlideOffset = pos;

	// This looks like a rebounding check for the angle
	float dyRot = (m_rotPrev.y - m_rot.y);
	if (dyRot < -180.0f)
		m_rotPrev.y += 360.0f;
	if (dyRot >= 180.0f)
		m_rotPrev.y -= 360.0f;
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

	m_rotPrev.x += m_rot.x - rotOld.x;
	m_rotPrev.y += m_rot.y - rotOld.y;
}

void Entity::reset()
{
	// TODO is this it
	m_posPrev = m_ySlideOffset = m_pos;
	m_rotPrev = m_rot;
	m_bRemoved = false;
	m_distanceFallen = 0.0f;
	field_D5 = false;
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

	field_90 = field_94;
	m_ySlideOffset = m_pos;
	field_B4++;
	m_rotPrev = m_rot;
	if (isInWater())
	{
		if (!field_D4 && !field_D6)
		{
			float dist = Mth::sqrt(m_vel.y * m_vel.y + m_vel.x * m_vel.x * 0.2f + m_vel.z * m_vel.z * 0.2f) * 0.2f;
			if (dist > 1.0f)
				dist = 1.0f;

			m_pLevel->playSound(this, "random.splash", dist, 1.0f + 0.4f * (sharedRandom.nextFloat() - sharedRandom.nextFloat()));

			float f1 = floorf(m_hitbox.min.y);

			for (int i = 0; float(i) < field_88 * 20.0f + 1.0f; i++)
			{
				m_pLevel->addParticle(
					"bubble",
					Vec3(
						m_pos.x + field_88 * (sharedRandom.nextFloat() * 2.0f - 1.0f),
						f1 + 1.0f,
						m_pos.z + field_88 * (sharedRandom.nextFloat() * 2.0f - 1.0f)
					),
					Vec3(
						m_vel.x,
						m_vel.y - 0.2f * sharedRandom.nextFloat(),
						m_vel.z
					)
				);
			}

			for (int i = 0; float(i) < field_88 * 20.0f + 1.0f; i++)
			{
				m_pLevel->addParticle(
					"splash",
					Vec3(
						m_pos.x + field_88 * (sharedRandom.nextFloat() * 2.0f - 1.0f),
						f1 + 1.0f,
						m_pos.z + field_88 * (sharedRandom.nextFloat() * 2.0f - 1.0f)
					),
					Vec3(
						m_vel.x,
						m_vel.y - 0.2f * sharedRandom.nextFloat(),
						m_vel.z
					)
				);
			}
		}

		field_D4 = true;
		m_fireTicks = 0;
		m_distanceFallen = 0;

		if (m_pLevel->m_bIsMultiplayer)
			goto label_4;
	}
	else
	{
		field_D4 = false;

		if (m_pLevel->m_bIsMultiplayer)
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

	if (field_D5)
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

	field_D6 = false;
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

	int data = m_pLevel->getData(tilePos);
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
	float x3 = 1.0f - this->field_B0;
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
	delete(itemInstance);
	// @TODO: not sure what this does, or is for
	itemEntity->m_ySlideOffset.x = 10;
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

void Entity::setRot(const Vec2& rot)
{
	m_rot.x = /*Mth::abs(rot.x) > 360.0f ? fmod(rot.x, 360.0f) :*/ rot.x;
	m_rot.y = /*Mth::abs(rot.y) > 360.0f ? fmod(rot.y, 360.0f) :*/ rot.y;
}

void Entity::setSize(float rad, float height)
{
	field_88 = rad;
	field_8C = height;
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

void Entity::resetPos()
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
	if (!field_D5)
		hurt(nullptr, 4);
}

void Entity::lavaHurt()
{
	if (!field_D5)
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

bool Entity::operator==(const Entity& other) const
{
	return m_EntityID == other.m_EntityID;
}
