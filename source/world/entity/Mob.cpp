#include "Mob.hpp"
/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Mob.hpp"
#include "world/level/Level.hpp"

Mob::Mob(Level* pLevel) : Entity(pLevel)
{
	field_DC = 10;
	field_E8 = 0.0f;
	field_EC = 0.0f;
	field_F0 = 0;
	field_F4 = 0.0f;
	field_F8 = 0.0f;
	m_health = 10;
	field_100 = 20;
	field_104 = 0;
	field_108 = 0;
	field_10C = 0.0f;
	field_110 = 0;
	field_114 = 0;
	field_118 = 0.0f;
	field_11C = 0.0f;
	field_120 = 0;
	field_124 = 0;
	field_128 = 0.0f;
	field_12C = 0.0f;
	field_130 = 0.0f;
	field_AFC = 0;
	field_B00 = 0.0f;
	field_B04 = 0.0f;
	field_B08 = 0.0f;
	field_B0C = 0;
	field_B10 = 0;
	field_B14 = 0.7f;
	field_B48 = 0;
	field_B4C = 0.0f;
	field_B50 = 0.0f;
	field_B54 = 0.0f;
	field_B58 = 0.0f;
	field_B5C = 0.0f;
	field_B60 = 1.0f;
	field_B64 = 0;
	field_B68 = 1;
	field_B69 = 0;
	field_B6C = 0;
	field_B70 = 0.0f;
	field_B74 = 0.0f;
	field_B78 = 0.0f;
	field_B7C = 0.0f;
	field_B80 = 0.0f;
	field_B84 = 0;
	m_pEntLookedAt = nullptr;

	m_texture = "/mob/pig.png";
	m_class = "";

	field_34 = 1;

	field_E4 = (Mth::random() + 1.0f) * 0.01f;
	setPos(m_pos.x, m_pos.y, m_pos.z);
	field_E0 = Mth::random() * 12398.0f;
	m_yaw = float(Mth::random() * M_PI);
	field_A8 = 0.5f;
}

Mob::~Mob()
{
}

void Mob::reset()
{
	Entity::reset();
	// TODO what fields to reset?
	m_health = 10;
}

void Mob::lerpTo(float x, float y, float z, float yaw, float pitch, int i)
{
	field_B70 = x;
	field_B74 = y + field_84;
	field_B78 = z;
	field_B7C = yaw;
	field_B80 = pitch;
	field_B6C = i;
}

void Mob::tick()
{
	superTick();

	if (field_B6C > 0)
	{
		float xPos = m_pos.x + (field_B70 - m_pos.x) / field_B6C;
		float yPos = m_pos.y + (field_B74 - m_pos.y) / field_B6C;
		float zPos = m_pos.z + (field_B78 - m_pos.z) / field_B6C;

		float ang = field_B7C - m_yaw;
		while (ang < -180.0f) ang += 360.0f;
		while (ang >= 180.0f) ang -= 360.0f;

		m_yaw += ang / float(field_B6C);
		m_pitch += (field_B80 - m_pitch) / float(field_B6C);

		setPos(xPos, yPos, zPos);
		setRot(m_yaw, m_pitch);

		field_B6C--;
	}

	aiStep();
	updateWalkAnim();

	//@TODO: untangle this variable mess
	float deltaX, deltaZ, dist, x1, x2, x3, x4, x5, x6, x7, field_E8_2, field_E8_new, v36;
	bool angleOOB = false;

	deltaX = m_pos.x - field_3C.x;
	deltaZ = m_pos.z - field_3C.z;
	dist = Mth::sqrt(deltaZ * deltaZ + deltaX * deltaX);
	field_E8_2 = field_E8;
	x1 = field_E8_2;

	field_B4C = field_B50;

	if (dist > 0.05f)
	{
		x2 = dist * 3.0f;
		v36 = Mth::atan2(deltaZ, deltaX);
		x3 = 1.0f;
		x1 = ((v36 * 180.0f) / float(M_PI)) - 90.0f;
		field_E8_2 = this->field_E8;
	}
	else
	{
		x3 = 0.0f;
		x2 = 0.0f;
	}

	x4 = field_F8;
	if (x4 <= 0.0f)
		x4 = m_yaw;
	else
		x4 = x1 = m_yaw;

	if (!field_7C)
		x3 = 0.0f;

	field_B50 += (x3 - field_B50) * 0.3f;
	
	x5 = x1 - field_E8_2;
	while (x5 < -180.0f)
		x5 += 360.0f;
	while (x5 >= 180.0f)
		x5 -= 360.0f;

	field_E8_new = field_E8_2 + (float)(x5 * 0.3);
	this->field_E8 = field_E8_new;

	x6 = x4 - field_E8_new;

	while (x6 < -180.0f)
		x6 += 360.0f;
	
	while (x6 >= 180.0f)
		x6 -= 360.0f;

	angleOOB = x6 < -90.0f || x6 >= 90.0f;

	if (x6 < -75.0f)
	{
		x7 = x4 + 75.0f;
		x6 = -75.0f;
		field_E8 = x4 + 75.0f;
		goto LABEL_30;
	}
	if (x6 >= 75.0f)
	{
		x7 = x4 - 75.0f;
		x6 = 75.0f;
		field_E8 = x4 - 75.0f;
	LABEL_30:
		field_E8 = x7 + (x6 * 0.2f);
		goto LABEL_31;
	}

	x7 = x4 - x6;
	field_E8 = x4 - x6;
	if (x6 * x6 > 2500.0f)
		goto LABEL_30;

LABEL_31:
	if (angleOOB)
		x2 = -x2;
	
	while (x4 - field_5C < -180.0f)
		field_5C -= 360.0f;

	while (x4 - field_5C >= 180.0f)
		field_5C += 360.0f;

	while (field_E8 - field_EC < -180.0f)
		field_EC -= 360.0f;

	while (field_E8 - field_EC >= 180.0f)
		field_EC += 360.0f;
	
	while (m_pitch - field_60 < -180.0f)
		field_60 -= 360.0f;

	while (m_pitch - field_60 >= 180.0f)
		field_60 += 360.0f;

	field_B54 += x2;
}

void Mob::baseTick()
{
	field_F4 = field_F8;
	Entity::baseTick();

	if (isAlive() && isInWall())
		hurt(nullptr, 1);

	if (isAlive() && isUnderLiquid(Material::water) && !isWaterMob())
	{
		field_BC--;
		if (field_BC == -20)
		{
			field_BC = 0;

			for (int i = 0; i < 8; i++)
			{
				m_pLevel->addParticle("bubble",
					m_pos.x + m_random.nextFloat() - m_random.nextFloat(),
					m_pos.y + m_random.nextFloat() - m_random.nextFloat(),
					m_pos.z + m_random.nextFloat() - m_random.nextFloat(),
					m_vel.x, m_vel.y, m_vel.z
				);
			}

			hurt(nullptr, 2);
		}
	}
	else
	{
		field_BC = field_D0;
	}

	field_118 = field_11C;

	if (field_114 > 0) field_114--;
	if (field_104 > 0) field_104--;
	if (field_B8  > 0) field_B8--;

	if (m_health <= 0)
	{
		field_110++;
		if (field_110 > 20)
		{
			beforeRemove();
			remove();
			for (int i = 0; i < 20; i++)
			{
				m_pLevel->addParticle("explode",
					m_pos.x + 2 * field_88 * m_random.nextFloat() - field_88,
					m_pos.y + field_8C * m_random.nextFloat(),
					m_pos.z + 2 * field_88 * m_random.nextFloat() - field_88,
					0.02f * (m_random.nextFloat() * 2 - 1) * (m_random.nextFloat() * 2 - 1),
					0.02f * (m_random.nextFloat() * 2 - 1) * (m_random.nextFloat() * 2 - 1),
					0.02f * (m_random.nextFloat() * 2 - 1) * (m_random.nextFloat() * 2 - 1)
				);
			}
		}
	}

	field_B58 = field_B54;
	field_EC = field_E8;
	field_5C = m_yaw;
	field_60 = m_pitch;
}

float Mob::getHeadHeight()
{
	return 0.85f * field_8C;
}

bool Mob::isPickable()
{
	return !m_bRemoved;
}

bool Mob::isPushable()
{
	return !m_bRemoved;
}

bool Mob::isShootable()
{
	return true;
}

bool Mob::isAlive()
{
	if (m_bRemoved)
		return false;

	return m_health >= 0;
}

bool Mob::hurt(Entity *pAttacker, int damage)
{
	if (m_pLevel->m_bIsMultiplayer)
		return false;

	field_AFC = m_pLevel->m_bIsMultiplayer;

	if (m_health <= 0)
		return false;

	field_12C = 1.5f;
	if (float(field_B8) <= float(field_DC) * 0.5f)
	{
		field_100 = m_health;
		field_B8 = field_DC;
		field_B84 = damage;
		actuallyHurt(damage);
		field_108 = 10;
		field_104 = 10;

		// not in 0.1
		markHurt();

		if (pAttacker)
		{
			float xd = pAttacker->m_pos.x - m_pos.x;
			float zd = pAttacker->m_pos.z - m_pos.z;

			while (zd * zd + xd * xd < 0.0001f)
			{
				xd = 0.01f * (Mth::random() - Mth::random());
				zd = 0.01f * (Mth::random() - Mth::random());
			}

			float ang = atan2f(zd, xd);
			v020_field_104 = ang * (180.0f / float(M_PI)) - m_yaw;

			knockback(pAttacker, damage, xd, zd);
		}
	}
	else
	{
		if (field_B84 >= damage)
			return 0;

		actuallyHurt(damage - field_B84);
		field_B84 = damage;
	}

	field_10C = 0;
	if (m_health <= 0)
		die(pAttacker);

	return true;
}

void Mob::animateHurt()
{
	field_108 = 10;
	field_10C = 0;
	field_104 = 10;
}

void Mob::setSize(float rad, float height)
{
	Entity::setSize(rad, height);
}

void Mob::outOfWorld()
{
	hurt(nullptr, 4);
}

void Mob::causeFallDamage(float level)
{
	int x = int(ceilf(level - 3));
	if (x > 0)
	{
		// from 0.2.0
		std::string fallSound;
		if (x > 4)
			fallSound = "damage.fallbig";
		else
			fallSound = "damage.fallsmall";

		m_pLevel->playSound(this, fallSound, 1.0f, 1.0f);

		hurt(nullptr, x);

		//@HUH: useless call to getTile? or could this be a return value of some sort
		//Entity::causeFallDamage returns nothing though, so....
		m_pLevel->getTile(Mth::floor(m_pos.x), Mth::floor(m_pos.y - 0.2f - field_84), Mth::floor(m_pos.z));
	}
}

void Mob::knockback(Entity* pEnt, int a, float x, float z)
{
	float power = Mth::invSqrt(x * x + z * z);

	m_vel.x = m_vel.x * 0.5f - x * power * 0.4f;
	m_vel.y = m_vel.y * 0.5f + 0.4f;
	m_vel.z = m_vel.z * 0.5f - z * power * 0.4f;

	//@BUG: maybe this is meant to behave differently.
	// Unless the player is falling, the Y vel will always be 0.4.
	if (m_vel.y > 0.4f)
		m_vel.y = 0.4f;
}

bool Mob::onLadder()
{
	int tileX = Mth::floor(m_pos.x);
	int tileZ = Mth::floor(m_pos.z);
	int tileY = Mth::floor(m_hitbox.min.y);

	//@INFO: Pre Beta 1.5 stair behaviour
	return
		m_pLevel->getTile(tileX, tileY + 0, tileZ) == Tile::ladder->m_ID || 
		m_pLevel->getTile(tileX, tileY + 1, tileZ) == Tile::ladder->m_ID;
}

void Mob::spawnAnim()
{

}

std::string Mob::getTexture()
{
	return m_texture;
}

void Mob::playAmbientSound()
{

}

int Mob::getAmbientSoundInterval()
{
	return 80;
}

void Mob::heal(int health)
{
	if (m_health <= 0)
		return;

	m_health += health;
	if (m_health > C_MAX_MOB_HEALTH)
		m_health = C_MAX_MOB_HEALTH;

	field_B8 = field_DC / 2;
}

HitResult Mob::pick(float f1, float f2)
{
	Vec3 pos = getPos(f2);
	Vec3 view = getViewVector(f2);

	Vec3 limit = pos + view * f1;
	return m_pLevel->clip(pos, limit);
}

void Mob::travel(float a2, float a3)
{
	float x1, x2, dragFactor, oldYPos = m_pos.y;
	if (isInWater())
	{
		moveRelative(a2, a3, 0.02f);
		move(m_vel.x, m_vel.y, m_vel.z);
		x1 = 0.8f;
		goto label_3;
	}
	if (isInLava())
	{
		moveRelative(a2, a3, 0.02f);
		move(m_vel.x, m_vel.y, m_vel.z);
		x1 = 0.5f;
	label_3:

		m_vel.y = m_vel.y * x1 - 0.02f;
		m_vel.x *= x1;
		m_vel.z *= x1;

		if (field_7D)
		{
			if (isFree(m_vel.x, m_vel.y + 0.6f - m_pos.y + oldYPos, m_vel.z))
				m_vel.y = 0.3f;
		}

		return;
	}

	if (!field_7C)
	{
		x2 = 0.02f;
	}
	else
	{
		float _x1;
		int tileX = Mth::floor(m_pos.x);
		int tileY = Mth::floor(m_hitbox.min.y);
		int tileZ = Mth::floor(m_pos.z);
		TileID tile = m_pLevel->getTile(tileX, tileY - 1, tileZ);
		if (tile <= 0)
			_x1 = 0.546f;
		else
			_x1 = Tile::tiles[tile]->field_30 * 0.91f;

		assert(_x1 != 0.0f);

		x2 = (0.16277f / (_x1 * _x1 * _x1)) * 0.1f;
	}

	moveRelative(a2, a3, x2);

	if (!field_7C)
	{
		dragFactor = 0.91f;
	}
	else
	{
		//@HUH: repeated code. Could be an inlined function?
		int tileX = Mth::floor(m_pos.x);
		int tileY = Mth::floor(m_hitbox.min.y);
		int tileZ = Mth::floor(m_pos.z);
		TileID tile = m_pLevel->getTile(tileX, tileY - 1, tileZ);
		if (tile <= 0)
			dragFactor = 0.546f;
		else
			dragFactor = Tile::tiles[tile]->field_30 * 0.91f;
	}

	if (onLadder())
	{
		m_distanceFallen = 0.0f;

		if (m_vel.y < -0.15f)
			m_vel.y = -0.15f;

		if (isSneaking() && m_vel.y < 0)
			m_vel.y = 0;
	}

	move(m_vel.x, m_vel.y, m_vel.z);

	if (field_7D && onLadder())
		m_vel.y = 0.2f;

	m_vel.y = (m_vel.y - 0.08f) * 0.98f; // gravity?

	// drag
	m_vel.x *= dragFactor;
	m_vel.z *= dragFactor;
}

void Mob::die(Entity* pCulprit)
{
	if (pCulprit && field_B48 > 0)
		pCulprit->awardKillScore(pCulprit, field_B48);

	field_B69 = true;

	if (!m_pLevel->m_bIsMultiplayer)
		dropDeathLoot();
}

bool Mob::canSee(Entity* pEnt)
{
	Vec3 v1 = m_pos;
	v1.y += getHeadHeight();
	Vec3 v2 = pEnt->m_pos;
	v2.y += pEnt->getHeadHeight();

	return m_pLevel->clip(v1, v2).m_hitType == HitResult::NONE;
}

void Mob::updateWalkAnim()
{
	field_128 = field_12C;

	float diffX = m_pos.x - field_3C.x;
	float diffZ = m_pos.z - field_3C.z;

	float spd = 4.0f * Mth::sqrt(diffX * diffX + diffZ * diffZ);
	if (spd > 1.0f)
		spd = 1.0f;

	field_12C += (spd - field_12C) * 0.4f;
	field_130 += field_12C;
}

void Mob::aiStep()
{
	if (isImmobile())
	{
		field_B0C = 0;
		field_B00 = 0.0f;
		field_B04 = 0.0f;
		field_B08 = 0.0f;
	}
	else if (!field_F0)
	{
		updateAi();
	}

	bool bIsInWater = isInWater(), bIsInLava = isInLava();
	if (field_B0C)
	{
		if (bIsInWater || bIsInLava)
			m_vel.y += 0.04f;
		else if (field_7C)
			jumpFromGround();
	}

	field_B00 *= 0.98f;
	field_B04 *= 0.98f;
	field_B08 *= 0.9f;

	travel(field_B00, field_B04);

	AABB aabb = m_hitbox;
	aabb.grow(0.2f, 0.2f, 0.2f);

	EntityVector* pEnts = m_pLevel->getEntities(this, aabb);
	for (EntityVector::iterator it = pEnts->begin(); it != pEnts->end(); it++)
	{
		Entity* pEnt = *it;
		if (pEnt->isPushable())
			pEnt->push(this);
	}
}

bool Mob::isWaterMob()
{
	return false;
}

void Mob::superTick()
{
	Entity::tick();
}

void Mob::lookAt(Entity* pEnt, float a3, float a4)
{
	float diffX = pEnt->m_pos.x - m_pos.x;
	float diffZ = pEnt->m_pos.z - m_pos.z;

	float q1 = (pEnt->m_hitbox.min.y + pEnt->m_hitbox.max.y) / 2 - (m_pos.y + getHeadHeight());
	float p1 = Mth::sqrt(diffX * diffX + diffZ * diffZ);

	float x1 = atan2f(diffZ, diffX);
	float x2 = atan2f(q1, p1);

	m_pitch = -rotlerp(m_pitch, x2 * 180.0f / float(M_PI), a4);
	m_yaw = rotlerp(m_yaw, x1 * 180.0f / float(M_PI) - 90.0f, a3);
}

bool Mob::isLookingAtAnEntity()
{
	return m_pEntLookedAt != nullptr;
}

Entity* Mob::getLookingAt()
{
	return m_pEntLookedAt;
}

void Mob::beforeRemove()
{

}

bool Mob::canSpawn()
{
	return m_pLevel->getCubes(this, m_hitbox)->empty();
}

float Mob::getAttackAnim(float f)
{
	float x = field_F8 - field_F4;
	
	if (x < 0.0f)
		x += 1.0f;

	return field_F4 + (x * f);
}

Vec3 Mob::getPos(float f)
{
	if (f == 1.0f)
		return m_pos;

	return Vec3(
        Mth::Lerp(field_3C.x, m_pos.x, f),
		Mth::Lerp(field_3C.y, m_pos.y, f),
		Mth::Lerp(field_3C.z, m_pos.z, f)
	);
}

Vec3 Mob::getLookAngle(float f)
{
	return getViewVector(1.0f);
}

Vec3 Mob::getViewVector(float f)
{
	constexpr float C_180_OVER_PI = 0.017453f;
	constexpr float C_PI = 3.1416f;
	
	if (f == 1.0)
	{
		float x1 = Mth::cos(-(m_yaw * C_180_OVER_PI) - C_PI);
		float x2 = Mth::sin(-(m_yaw * C_180_OVER_PI) - C_PI);
		float x3 = -Mth::cos(-(m_pitch * C_180_OVER_PI));

		return Vec3(x1 * x3, Mth::sin(-(m_pitch * C_180_OVER_PI)), x2 * x3);
	}

	float x1 = field_60 + (m_pitch - field_60) * f;
	float x2 = -((field_5C + (m_yaw - field_5C) * f) * C_180_OVER_PI) - C_PI;
	float x3 = Mth::cos(x2);
	float x4 = Mth::sin(x2);
	float x5 = -(x1 * C_180_OVER_PI);
	float x6 = -Mth::cos(x5);

	return Vec3(x4 * x6, Mth::sin(x5), x3 * x6);
}

int Mob::getMaxSpawnClusterSize()
{
	return 4;
}

bool Mob::isBaby()
{
	return false;
}

void Mob::actuallyHurt(int damage)
{
#ifdef TEST_SURVIVAL_MODE
	m_health -= damage;
#endif
}

bool Mob::removeWhenFarAway()
{
	return true;
}

int Mob::getDeathLoot()
{
	return 0;
}

void Mob::dropDeathLoot()
{

}

bool Mob::isImmobile()
{
	return m_health <= 0;
}

void Mob::jumpFromGround()
{
	m_vel.y = 0.42f;
}

void Mob::updateAi()
{
	field_AFC++;

	Entity* nearestPlayer = m_pLevel->getNearestPlayer(this, -1.0f);

	// if we need to remove ourselves when far away, and there's a player around
	// (if there's no players, we're on a headless server)
	if (removeWhenFarAway() && nearestPlayer)
	{
		float distSqr = nearestPlayer->distanceToSqr_inline(m_pos.x, m_pos.y, m_pos.z);
		if (distSqr > 9216.0f)
			remove();

		if (field_AFC > 600 && m_random.nextInt(800) == 0)
		{
			if (distSqr >= 1024.0f)
				remove();
			else
				field_AFC = 0;
		}
	}

	field_B00 = 0.0f;
	field_B04 = 0.0f;

	if (m_random.nextFloat() < 0.02f)
	{
		Entity* nearestPlayer = m_pLevel->getNearestPlayer(this, 8.0f);
		if (nearestPlayer)
		{
			m_pEntLookedAt = nearestPlayer;

			field_120 = m_random.nextInt(20) + 10;
		}
		else
		{
			field_B08 = (m_random.nextFloat() - 0.5f) * 20.0f;
		}
	}

	if (m_pEntLookedAt)
	{
		lookAt(m_pEntLookedAt, 10.0f, getMaxHeadXRot());

		// gaze timer
		field_120--;

		// if the entity was removed, or we're too far away, or our gaze timer is up
		if (field_120 < 0 || m_pEntLookedAt->m_bRemoved || m_pEntLookedAt->distanceToSqr(this) > 64.0f)
			// stop staring
			m_pEntLookedAt = nullptr;
	}
	else
	{
		if (m_random.nextFloat() < 0.05f)
			field_B08 = (m_random.nextFloat() - 0.5f) * 20.0f;

		m_yaw += field_B08;
		m_pitch = field_B10;
	}

	if (isInWater() || isInLava())
	{
		field_B0C = m_random.nextFloat() < 0.8f;
	}
}

int Mob::getMaxHeadXRot()
{
	return 10;
}

int Mob::getMaxHealth()
{
	return 10;
}

float Mob::getSoundVolume()
{
	return 1.0f;
}

std::string Mob::getAmbientSound()
{
	return "";
}

std::string Mob::getHurtSound()
{
	return "random.hurt";
}

std::string Mob::getDeathSound()
{
	return "random.hurt";
}

float Mob::getWalkingSpeedModifier()
{
	return 0.7f;
}

void Mob::defineSynchedData()
{

}

float Mob::rotlerp(float a2, float a3, float a4)
{
	float ang = a3 - a2;
	while (ang < -180.0f) ang += 360.0f;
	while (ang >= 180.0f) ang -= 360.0f;
	if (ang > a4)
		ang = a4;
	if (ang < -a4)
		ang = -a4;
	return a2 + ang;
}
