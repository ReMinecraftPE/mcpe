/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Entity.hpp"
#include "Player.hpp"
#include "Level.hpp"

int Entity::entityCounter;
Random Entity::sharedRandom;

Entity::Entity(Level* pLevel)
{
	m_pLevel = pLevel;
	m_EntityID = ++entityCounter;
	setPos(0, 0, 0);
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

void Entity::setPos(float x, float y, float z)
{
	m_pos = Vec3(x, y, z);

	float halfSize = field_88 / 2;
	float lowY = y - field_84 + field_A4;

	m_hitbox = AABB(
		x - halfSize,
		lowY,
		z - halfSize,
		x + halfSize,
		lowY + field_8C,
		z + halfSize);
}

void Entity::remove()
{
	m_bRemoved = true;
}

int Entity::move(float x, float y, float z)
{
	float x_1 = x, z_1 = z;

	if (m_bNoCollision)
	{
		// just move it. Don't perform any kind of collision
		m_hitbox.min.x += x;
		m_hitbox.max.x += x;
		m_hitbox.min.y += y;
		m_hitbox.max.y += y;
		m_hitbox.min.z += z;
		m_hitbox.max.z += z;

		m_pos.x = (m_hitbox.max.x + m_hitbox.min.x) / 2;
		m_pos.z = (m_hitbox.max.z + m_hitbox.min.z) / 2;
		m_pos.y = m_hitbox.min.y + field_84 - field_A4;

		return 1300;
	}

	//@TODO: untangle the control flow

	float x1, x2, x3, x4, x5, x6;
	float x7, x8, x9, x10, x11, x12, x20;
	float x_2, z_2, x_3, z_3;
	float oldX, oldZ;
	bool b1, b2, b3, b4, b5, b6;
	AABB hitold;

	oldX = m_pos.x, oldZ = m_pos.z;

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

	if (!field_7C)
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

			auto cubes = m_pLevel->getCubes(this, aabb);

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
		}
		while (x_1 != 0.0f);

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

			auto cubes = m_pLevel->getCubes(this, aabb);

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
		}
		while (z_1 != 0.0f);
		b3 = z_1 < 0.0f;
		b4 = z_1 > 0.0f;
	}

	b5 = true;

label_5:
	if (b1) x6 += x_1;
	if (b2) x3 += x_1;
	if (y < 0.0f) x5 += y;
	if (y > 0.0f) x2 += y;
	if (b3) x4 += z;
	if (b4) x1 += z;

	AABB scanAABB(x6, x5, x4, x3, x2, x1);
	auto pCubes = m_pLevel->getCubes(this, scanAABB);

	float newY = y;
	for (const auto &aabb : (*pCubes))
		newY = aabb.clipYCollide(m_hitbox, newY);

	m_hitbox.move(0, newY, 0);

	if (!field_81 && newY != y)
	{
		z_1 = 0.0f;
		x_1 = 0.0f;
		newY = 0.0f;
	}

	if (field_7C)
	{
		b6 = true;
	}
	else
	{
		b6 = y < 0.0f;
		if (newY == y)
			b6 = 0;
	}

	for (const auto& aabb : (*pCubes))
		x_1 = aabb.clipXCollide(m_hitbox, x_1);

	m_hitbox.move(x_1, 0, 0);

	if (!field_81 && x_1 != x_2)
	{
		z_1 = 0.0f;
		x_1 = 0.0f;
		newY = 0.0f;
	}

	for (const auto& aabb : (*pCubes))
		z_1 = aabb.clipZCollide(m_hitbox, z_1);

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

	if (field_A4 >= 0.05f || x_2 == x_1 && z_2 == z_1)
		goto label_44;

	// oh come on, undoing all our work??
	hitold = m_hitbox;
	m_hitbox = AABB(x12, x11, x10, x9, x8, x7);

	if (b1) x12 += x_2;
	if (b2) x9  += x_2;
	x8 += x20; //@BUG: missing if (x20 > 0) check?
	if (x20 < 0.0f) x11 += x20;
	if (b3) x10 += z_2;
	if (b4) x7  += z_2;

	{
		AABB scanAABB2(x12, x11, x10, x9, x8, x7);

		//@BUG: useless copy
		//@BUG: this doesn't actually copy anything
		*pCubes = *m_pLevel->getCubes(this, scanAABB2);
	}

	for (const auto& aabb : (*pCubes))
		x20 = aabb.clipYCollide(m_hitbox, x20);

	m_hitbox.move(0, x20, 0);

	if (field_81 || x20 == y)
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

	for (const auto& aabb : (*pCubes))
		x_3 = aabb.clipXCollide(m_hitbox, x_3);

	m_hitbox.move(x_3, 0, 0);

	if (!field_81 && x_2 != x_3)
	{
		x20 = 0.0f;
		z_3 = 0.0f;
		x_3 = 0.0f;
	}

	for (const auto& aabb : (*pCubes))
		z_3 = aabb.clipZCollide(m_hitbox, z_3);

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
	m_pos.y = m_hitbox.min.y - field_A4 + field_84;

	field_7D = x_2 != x_3 || z_2 != z_3;
	field_7F = field_7D || newY != y;
	field_7C = y < 0.0f && newY != y;
	field_7E = newY != y;

	checkFallDamage(newY, field_7C);

	if (x_2 != x_3) m_vel.x = 0.0;
	if (newY != y)  m_vel.y = 0.0;
	if (z_2 != z_3) m_vel.z = 0.0;

	if (!b5)
	{
		float diffX = (m_pos.x - oldX), diffZ = (m_pos.z - oldZ);
		field_94 += Mth::sqrt(diffZ * diffZ + diffX * diffX) * 0.6f;

		int tileX = Mth::floor(m_pos.x);
		int tileY = Mth::floor(m_pos.y - 0.2f - field_84);
		int tileZ = Mth::floor(m_pos.z);

		TileID tileID = m_pLevel->getTile(tileX, tileY, tileZ);

		if (tileID > 0 && field_94 > float(field_D8))
		{
			++field_D8;
			bool bPlaySound = true;

			auto sound = Tile::tiles[tileID]->m_pSound;
			if (m_pLevel->getTile(tileX, tileY + 1, tileZ) == Tile::topSnow->m_ID)
				sound = Tile::topSnow->m_pSound;
			else if (Tile::tiles[tileID]->m_pMaterial->isLiquid())
				bPlaySound = false;

			if (bPlaySound)
				m_pLevel->playSound(this, "step." + sound->m_name, sound->field_18 * 0.15f, sound->field_1C);

			Tile::tiles[tileID]->stepOn(m_pLevel, tileX, tileY, tileZ, this);
		}
	}

	// Check whether the entity is inside of any tiles.

	int minX = Mth::floor(m_hitbox.min.x), minY = Mth::floor(m_hitbox.min.y), minZ = Mth::floor(m_hitbox.min.z),
		maxX = Mth::floor(m_hitbox.max.x), maxY = Mth::floor(m_hitbox.max.y), maxZ = Mth::floor(m_hitbox.max.z);

	if (m_pLevel->hasChunksAt(minX, minY, minZ, maxX, maxY, maxZ))
	{
		for (int x = minX; x <= maxX; x++)
			for (int y = minY; y <= maxY; y++)
				for (int z = minZ; z <= maxZ; z++)
				{
					TileID tileID = m_pLevel->getTile(x, y, z);
					if (tileID > 0)
						Tile::tiles[tileID]->entityInside(m_pLevel, x, y, z, this);
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
			if (field_C0 > 0)
				field_C0 = -field_C4;

			return 1300;
		}

		field_C0++;
		if (field_C0 == 0)
			field_C0 = 300;
	}
	else
	{
		if (field_C0 <= 0)
			field_C0 = -field_C4;

		if (bIsInWater)
			goto label_76;
	}

	return 1300;
}

void Entity::moveTo(float x, float y, float z, float yaw, float pitch)
{
	y += field_84;

	m_pos.x = x;
	m_yaw = yaw;
	field_3C.x = x;
	field_98.x = x;

	m_pos.z = z;
	m_pitch = pitch;
	field_3C.z = z;
	field_98.z = z;

	m_pos.y = y;
	field_3C.y = y;
	field_98.y = y;

	setPos(x, y, z);
}

void Entity::absMoveTo(float x, float y, float z, float yaw, float pitch)
{
	field_A4 = 0.0f;

	field_5C = yaw;
	m_yaw    = yaw;
	m_pitch  = pitch;
	field_60 = pitch;

	m_pos.x = x;
	field_3C.x = x;
	m_pos.y = y;
	field_3C.y = y;
	m_pos.z = z;
	field_3C.z = z;

	//@WHAT: (yaw - yaw) == 0
	if (yaw - yaw < -180.0f)
		field_5C = yaw + 360.0f;
	if (yaw - yaw >= 180.0f)
		field_5C = field_5C - 360.0f;

	setPos(this->m_pos.x, this->m_pos.y, this->m_pos.z);
	return setRot(yaw, pitch);
}

// Y is some sort of scalar value, not related to the position you're moving to
void Entity::moveRelative(float x, float z, float y)
{
#ifdef ORIGINAL_CODE
	float d = sqrtf(x * x + z * z);
	if (d < 0.01f) return;
	if (d > 1.0f)
		d = 1.0f;

	y /= d;
	x *= y;
	z *= y;
#else
	// length of input vector
	float d = sqrtf((x * x) + (z * z));
	if (d < 0.01f) return;

	// normalization factor
	float factor = 1.0f / d;

	// cap length
	if (d > 1.0f)
		d = 1.0f;

	// apply factor * length
	x *= factor * d * y;
	z *= factor * d * y;
#endif

	float yaw = m_yaw * float(M_PI);
	float syaw = sinf(yaw / 180.0f);
	float cyaw = cosf(yaw / 180.0f);

	m_vel.x += x * cyaw - z * syaw;
	m_vel.z += x * syaw + z * cyaw;
}

void Entity::lerpTo(float x, float y, float z, float yaw, float pitch, int p)
{
	setPos(x, y, z);
	setRot(yaw, pitch);
}

void Entity::lerpMotion(float x, float y, float z)
{
	m_vel = Vec3(x, y, z);
}

void Entity::turn(float yaw, float pitch)
{
	float y_old = m_yaw;
	float p_old = m_pitch;

	m_yaw   += yaw   * 0.15f;
	m_pitch -= pitch * 0.15f;

	// can't rotate more than facing fully up or fully down
	if (m_pitch < -90.0f)
		m_pitch = -90.0f;
	if (m_pitch > 90.0f)
		m_pitch = 90.0f;

	field_5C += m_yaw   - y_old;
	field_60 += m_pitch - p_old;
}

void Entity::interpolateTurn(float yaw, float pitch)
{
	m_yaw   += yaw   * 0.15f;
	m_pitch -= pitch * 0.15f;

	// can't rotate more than facing fully up or fully down
	if (m_pitch < -90.0f)
		m_pitch = -90.0f;
	if (m_pitch > 90.0f)
		m_pitch = 90.0f;
}

void Entity::tick()
{
	baseTick();
}

void Entity::baseTick()
{
	//@TODO: untangle the gotos

	field_90 = field_94;
	field_3C = m_pos;
	field_B4++;
	field_5C = m_yaw;
	field_60 = m_pitch;
	if (isInWater())
	{
		if (!field_D4 && !field_D6)
		{
			float dist = sqrtf(m_vel.y * m_vel.y + m_vel.x * m_vel.x * 0.2f + m_vel.z * m_vel.z * 0.2f) * 0.2f;
			if (dist > 1.0f)
				dist = 1.0f;

			m_pLevel->playSound(this, "random.splash", dist, 1.0f + 0.4f * (sharedRandom.nextFloat() - sharedRandom.nextFloat()));

			float f1 = floorf(m_hitbox.min.y);

			for (int i = 0; float(i) < field_88 * 20.0f + 1.0f; i++)
			{
				m_pLevel->addParticle(
					"bubble",
					m_pos.x + field_88 * (sharedRandom.nextFloat() * 2.0f - 1.0f),
					f1 + 1.0f,
					m_pos.z + field_88 * (sharedRandom.nextFloat() * 2.0f - 1.0f),
					m_vel.x,
					m_vel.y - 0.2f * sharedRandom.nextFloat(),
					m_vel.z
				);
			}

			for (int i = 0; float(i) < field_88 * 20.0f + 1.0f; i++)
			{
				m_pLevel->addParticle(
					"splash",
					m_pos.x + field_88 * (sharedRandom.nextFloat() * 2.0f - 1.0f),
					f1 + 1.0f,
					m_pos.z + field_88 * (sharedRandom.nextFloat() * 2.0f - 1.0f),
					m_vel.x,
					m_vel.y - 0.2f * sharedRandom.nextFloat(),
					m_vel.z
				);
			}
		}

		field_D4 = true;
		field_C0 = 0;
		m_distanceFallen = 0;

		if (m_pLevel->field_11)
			goto label_4;
	}
	else
	{
		field_D4 = false;

		if (m_pLevel->field_11)
		{
		label_4:
			field_C0 = 0;
			goto label_5;
		}
	}

	if (field_C0 <= 0)
	{
	label_5:
		if (!isInLava())
			goto label_6;
		goto label_15;
	}

	if (field_D5)
	{
		field_C0 -= 4;
		if (field_C0 < 0)
			field_C0 = 0;
		goto label_5;
	}

	if (field_C0 % 20 == 0)
	{
		hurt(nullptr, true);
	}

	field_C0--;

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

bool Entity::intersects(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
	AABB aabb(minX, minY, minZ, maxX, maxY, maxZ);
	return aabb.intersect(m_hitbox);
}

bool Entity::isFree(float offX, float offY, float offZ)
{
	AABB aabb = m_hitbox;
	aabb.move(offX, offY, offZ);

	auto pCubes = m_pLevel->getCubes(this, aabb);
	if (!pCubes)
		return false;

	return !m_pLevel->containsAnyLiquid(aabb);
}

bool Entity::isFree(float offX, float offY, float offZ, float expand)
{
	AABB aabb = m_hitbox;
	aabb.move(offX, offY, offZ);
	aabb.grow(expand, expand, expand);

	auto pCubes = m_pLevel->getCubes(this, aabb);
	if (!pCubes)
		return false;

	return !m_pLevel->containsAnyLiquid(aabb);
}

bool Entity::isInWall()
{
	return m_pLevel->isSolidTile(Mth::floor(m_pos.x), Mth::floor(m_pos.y + getHeadHeight()), Mth::floor(m_pos.z));
}

bool Entity::isInWater()
{
	AABB aabb = m_hitbox;
	aabb.grow(0, -0.4f, 0);
	return m_pLevel->checkAndHandleWater(aabb, Material::water, this);
}

bool Entity::isInLava()
{
	AABB aabb = m_hitbox;
	aabb.grow(-0.1f, -0.4f, -0.1f);
	return m_pLevel->containsMaterial(aabb, Material::lava);
}

bool Entity::isUnderLiquid(Material* pMtl)
{
	int tileX = Mth::floor(m_pos.x);
	int tileY = Mth::floor(m_pos.y + getHeadHeight());
	int tileZ = Mth::floor(m_pos.z);

	Tile* pTile = Tile::tiles[m_pLevel->getTile(tileX, tileY, tileZ)];
	if (!pTile || pTile->m_pMaterial != pMtl)
		return false;

	int data = m_pLevel->getData(tileX, tileY, tileZ);
	int level = data <= 7 ? data + 1 : 1;

	return float(tileY) < float(tileY + 1) - (float(level) / 9.0f - 0.11111f);
}

float Entity::getHeadHeight()
{
	return 0.0f;
}

float Entity::getShadowHeightOffs()
{
	return field_8C / 2.0f;
}

float Entity::getBrightness(float f)
{
	int tileX = Mth::floor(m_pos.x);
	int tileY = Mth::floor(m_pos.y - field_84 + (m_hitbox.max.y - m_hitbox.min.y) * 0.66f);
	int tileZ = Mth::floor(m_pos.z);

	int minX = Mth::floor(m_hitbox.min.x);
	int minY = Mth::floor(m_hitbox.min.y);
	int minZ = Mth::floor(m_hitbox.min.z);
	int maxX = Mth::floor(m_hitbox.max.x);
	int maxY = Mth::floor(m_hitbox.max.y);
	int maxZ = Mth::floor(m_hitbox.max.z);

	if (!m_pLevel->hasChunksAt(minX, minY, minZ, maxX, maxY, maxZ))
		return 0;

	return m_pLevel->getBrightness(tileX, tileY, tileZ);
}

float Entity::distanceTo(Entity* pEnt)
{
	return distanceTo(pEnt->m_pos.x, pEnt->m_pos.y, pEnt->m_pos.z);
}

float Entity::distanceTo(float x, float y, float z)
{
	return sqrtf(distanceToSqr(x, y, z));
}

float Entity::distanceToSqr(Entity* pEnt)
{
	return distanceToSqr(pEnt->m_pos.x, pEnt->m_pos.y, pEnt->m_pos.z);
}

float Entity::distanceToSqr(float x, float y, float z)
{
	float diffX = m_pos.x - x;
	float diffY = m_pos.y - y;
	float diffZ = m_pos.z - z;

	return diffX * diffX + diffY * diffY + diffZ * diffZ;
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
	
	float x1 = sqrtf(maxDiff);
	float x2 = 1.0f / x1;
	if (x2 > 1.0f)
		x2 = 1.0f;
	float x3 = 1.0f - this->field_B0;
	float x4 = x3 * diffX / x1 * x2 * 0.05f;

	push(-x4, 0.0f, -x4);
	bud->push(x4, 0.0f, x4);
}

void Entity::push(float x, float y, float z)
{
	m_vel.x += x;
	m_vel.y += y;
	m_vel.z += z;
}

bool Entity::isPickable()
{
	return false;
}

bool Entity::isPushable()
{
	return false;
}

bool Entity::isShootable()
{
	return false;
}

bool Entity::isSneaking()
{
	return false;
}

bool Entity::isAlive()
{
	return m_bRemoved;
}

bool Entity::isOnFire()
{
	return field_C0 > 0;
}

bool Entity::isPlayer()
{
	return false;
}

bool Entity::isCreativeModeAllowed()
{
	return false;
}

bool Entity::shouldRender(Vec3& camPos)
{
	return shouldRenderAtSqrDistance(distanceToSqr(camPos.x, camPos.y, camPos.z));
}

bool Entity::shouldRenderAtSqrDistance(float distSqr)
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

float Entity::getPickRadius()
{
	return 0.1f;
}


void Entity::awardKillScore(Entity* pKilled, int score)
{
	
}

void Entity::setEquippedSlot(int a, int b, int c)
{

}

void Entity::setRot(float yaw, float pitch)
{
	m_yaw   = yaw;
	m_pitch = pitch;
}

void Entity::setSize(float rad, float height)
{
	field_88 = rad;
	field_8C = height;
}

void Entity::setPos(EntityPos* pPos)
{
	if (pPos->m_bHasPos)
		setPos(pPos->m_pos.x, pPos->m_pos.y, pPos->m_pos.z);
	else
		setPos(m_pos.x, m_pos.y, m_pos.z);

	if (pPos->m_bHasRot)
		setRot(pPos->m_yaw, pPos->m_pitch);
	else
		setRot(m_yaw, m_pitch);
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
		setPos(m_pos.x, m_pos.y, m_pos.z);

		auto pCubes = m_pLevel->getCubes(this, m_hitbox);

		// if we aren't inside any tiles, great!
		if (!pCubes->size())
			break;

		m_pos.y += 1.0f;
	}

	m_vel.x = 0.0f;
	m_vel.y = 0.0f;
	m_vel.z = 0.0f;
	m_pitch = 0.0f;
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
		field_C0 = 600;
	}
}

int Entity::hashCode()
{
	return m_EntityID;
}

bool Entity::operator==(const Entity& other) const
{
	return m_EntityID == other.m_EntityID;
}
