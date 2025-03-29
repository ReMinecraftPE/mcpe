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
	m_invulnerableDuration = 10;
	field_E8 = 0.0f;
	field_EC = 0.0f;
	field_F0 = 0;
	m_oAttackAnim = 0.0f;
	m_attackAnim = 0.0f;
	m_health = 10;
	m_lastHealth = 20;
	m_hurtTime = 0;
	m_hurtDuration = 0;
	m_hurtDir = 0.0f;
	field_110 = 0;
	field_114 = 0;
	field_118 = 0.0f;
	field_11C = 0.0f;
	field_120 = 0;
	field_124 = 0;
	field_128 = 0.0f;
	m_walkAnimSpeed = 0.0f;
	field_130 = 0.0f;
	field_AFC = 0;
	field_B00 = Vec2::ZERO;
	field_B08 = 0.0f;
	m_bJumping = 0;
	field_B10 = 0;
	m_runSpeed = 0.7f;
	field_B48 = 0;
	field_B4C = 0.0f;
	field_B50 = 0.0f;
	field_B54 = 0.0f;
	field_B58 = 0.0f;
	m_rotOffs = 0.0f;
	field_B60 = 1.0f;
	field_B64 = 0;
	field_B68 = 1;
	field_B69 = 0;
	m_lSteps = 0;
	m_lPos = Vec3::ZERO;
	m_lRot = Vec2::ZERO;
	m_lastHurt = 0;
	m_pEntLookedAt = nullptr;
	m_bSwinging = false;
	m_swingTime = 0;
	m_ambientSoundTime = 0;

	m_texture = "/mob/pig.png";
	m_class = "";

	m_bBlocksBuilding = true;

	field_E4 = (Mth::random() + 1.0f) * 0.01f;
	setPos(m_pos);
	field_E0 = Mth::random() * 12398.0f;
	m_rot.x = float(Mth::random() * M_PI);
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

void Mob::lerpTo(const Vec3& pos, const Vec2& rot, int steps)
{
	m_lPos = pos;
	m_lPos.y += m_heightOffset;
	m_lRot = rot;
	m_lSteps = steps;
}

void Mob::tick()
{
	superTick();

	if (m_lSteps > 0)
	{
		setPos(Vec3(m_pos.x + ((m_lPos.x - m_pos.x) / (float)m_lSteps),
			        m_pos.y + ((m_lPos.y - m_pos.y) / (float)m_lSteps),
			        m_pos.z + ((m_lPos.z - m_pos.z) / (float)m_lSteps)));

		// Similar to rotlerp
		// I'm pretty sure this is super inefficient and its trying to do what I have it doing in setRot already.
		float ang = m_lRot.x - m_rot.x;
		while (ang < -180.0f) ang += 360.0f;
		while (ang >= 180.0f) ang -= 360.0f;

		setRot(Vec2(m_rot.x + ((m_lRot.x - m_rot.x) / float(m_lSteps)),
			        m_rot.y + ((m_lRot.y - m_rot.y) / float(m_lSteps))));

		m_lSteps--;
	}

	aiStep();
	updateWalkAnim();

	//@TODO: untangle this variable mess
	float dist, x1, x2, x3, x4, x5, x6, x7, field_E8_2, field_E8_new, v36;
	bool angleOOB = false;

	Vec3 delta = m_pos - m_oPos;
	dist = Mth::sqrt(delta.z * delta.z + delta.x * delta.x);
	field_E8_2 = field_E8;
	x1 = field_E8_2;

	field_B4C = field_B50;

	if (dist > 0.05f)
	{
		x2 = dist * 3.0f;
		v36 = Mth::atan2(delta.z, delta.x);
		x3 = 1.0f;
		x1 = ((v36 * 180.0f) / float(M_PI)) - 90.0f;
		field_E8_2 = this->field_E8;
	}
	else
	{
		x3 = 0.0f;
		x2 = 0.0f;
	}

	x4 = m_attackAnim;
	if (x4 <= 0.0f)
		x4 = m_rot.x;
	else
		x4 = x1 = m_rot.x;

	if (!m_onGround)
		x3 = 0.0f;

	field_B50 += (x3 - field_B50) * 0.3f;
	

	// Similar to rotlerp
	// I'm pretty sure this is super inefficient and its trying to do what I have it doing in setRot already.
	x5 = x1 - field_E8_2;
	while (x5 < -180.0f)
		x5 += 360.0f;
	while (x5 >= 180.0f)
		x5 -= 360.0f;

	field_E8_new = field_E8_2 + (float)(x5 * 0.3);
	this->field_E8 = field_E8_new;

	x6 = x4 - field_E8_new;


	// Similar to rotlerp
	// I'm pretty sure this is super inefficient and its trying to do what I have it doing in setRot already.
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

	// Similar to rotlerp
	// I'm pretty sure this is super inefficient and its trying to do what I have it doing in setRot already.
	while (x4 - m_rotPrev.x < -180.0f)
		m_rotPrev.x -= 360.0f;

	while (x4 - m_rotPrev.x >= 180.0f)
		m_rotPrev.x += 360.0f;

	while (field_E8 - field_EC < -180.0f)
		field_EC -= 360.0f;

	while (field_E8 - field_EC >= 180.0f)
		field_EC += 360.0f;
	
	while (m_rot.y - m_rotPrev.y < -180.0f)
		m_rotPrev.y -= 360.0f;

	while (m_rot.y - m_rotPrev.y >= 180.0f)
		m_rotPrev.y += 360.0f;

	field_B54 += x2;
}

void Mob::baseTick()
{
	m_oAttackAnim = m_attackAnim;
	Entity::baseTick();

	if (m_random.nextInt(1000) < m_ambientSoundTime++)
	{
		playAmbientSound();
	}

	if (isAlive() && isInWall())
		hurt(nullptr, 1);

	// Java
	/*if (m_bFireImmune || m_pLevel->m_bIsMultiplayer)
	{
		m_fireTicks = 0;
	}*/


	if (isAlive() && isUnderLiquid(Material::water) && !isWaterMob())
	{
		m_airCapacity--;
		if (m_airCapacity == -20)
		{
			m_airCapacity = 0;

			for (int i = 0; i < 8; i++)
			{
				m_pLevel->addParticle("bubble",
					Vec3(
						m_pos.x + m_random.nextFloat() - m_random.nextFloat(),
						m_pos.y + m_random.nextFloat() - m_random.nextFloat(),
						m_pos.z + m_random.nextFloat() - m_random.nextFloat()
					),
					m_vel
				);
			}

			hurt(nullptr, 2);
		}
	}
	else
	{
		m_airCapacity = m_airSupply;
	}

	field_118 = field_11C;

	if (field_114 > 0) field_114--;
	if (m_hurtTime > 0) m_hurtTime--;
	if (m_invulnerableTime > 0) m_invulnerableTime--;

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
					Vec3(
						m_pos.x + 2 * m_bbWidth * m_random.nextFloat() - m_bbWidth,
						m_pos.y + m_bbHeight * m_random.nextFloat(),
						m_pos.z + 2 * m_bbWidth * m_random.nextFloat() - m_bbWidth
					),
					Vec3(
						0.02f * (m_random.nextFloat() * 2 - 1) * (m_random.nextFloat() * 2 - 1),
						0.02f * (m_random.nextFloat() * 2 - 1) * (m_random.nextFloat() * 2 - 1),
						0.02f * (m_random.nextFloat() * 2 - 1) * (m_random.nextFloat() * 2 - 1)
					)
				);
			}
		}
	}

	field_B58 = field_B54;
	field_EC = field_E8;
	m_rotPrev = m_rot;
}

bool Mob::isAlive() const
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

	m_walkAnimSpeed = 1.5f;
	bool var3 = true;
	if (float(m_invulnerableTime) > float(m_invulnerableDuration) / 2.0f)
	{
		if (damage <= m_lastHurt)
			return false;

		actuallyHurt(damage - m_lastHurt);
		m_lastHurt = damage;
		var3 = false;
	}
	else
	{
		m_lastHurt = damage;
		m_lastHealth = m_health;
		m_invulnerableTime = m_invulnerableDuration;
		actuallyHurt(damage);
		m_hurtTime = m_hurtDuration = 10;
	}

	m_hurtDir = 0.0f;
	// not in 0.1
	if (var3)
	{
		//m_pLevel->broadcastEntityEvent(this, 2); // Java
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
			v020_field_104 = ang * (180.0f / float(M_PI)) - m_rot.x;

			knockback(pAttacker, damage, xd, zd);
		}
	}

	if (m_health <= 0)
	{
		if (var3)
		{
			m_pLevel->playSound(this, getDeathSound(), getSoundVolume(), (m_random.nextFloat() - m_random.nextFloat()) * 0.2f + 1.0f);
		}

		die(pAttacker);
	}
	else if (var3)
	{
		m_pLevel->playSound(this, getHurtSound(), getSoundVolume(), (m_random.nextFloat() - m_random.nextFloat()) * 0.2f + 1.0f);
	}


	return true;
}

void Mob::animateHurt()
{
	m_hurtDuration = 10;
	m_hurtDir = 0;
	m_hurtTime = 10;
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

		m_pLevel->playSound(this, fallSound);

		hurt(nullptr, x);

		//@HUH: useless call to getTile? or could this be a return value of some sort
		//Entity::causeFallDamage returns nothing though, so....
		m_pLevel->getTile(TilePos(m_pos.x, m_pos.y - 0.2f - m_heightOffset, m_pos.z));
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

bool Mob::onLadder() const
{
	TilePos tilePos = TilePos(m_pos.x, m_hitbox.min.y, m_pos.z);

	//@INFO: Pre Beta 1.5 stair behaviour
	return
		m_pLevel->getTile(tilePos) == Tile::ladder->m_ID || 
		m_pLevel->getTile(tilePos.above()) == Tile::ladder->m_ID;
}

void Mob::spawnAnim()
{

}

std::string Mob::getTexture() const
{
	return m_texture;
}

void Mob::playAmbientSound()
{
	m_ambientSoundTime = -getAmbientSoundInterval();
	std::string sound = getAmbientSound();
	if (sound != "")
	{
		m_pLevel->playSound(this, sound, getSoundVolume(), (m_random.nextFloat() - m_random.nextFloat()) * 0.2f + 1.0f);
	}
}

int Mob::getAmbientSoundInterval() const
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

	m_invulnerableTime = m_invulnerableDuration / 2;
}

HitResult Mob::pick(float f1, float f2)
{
	Vec3 pos = getPos(f2);
	Vec3 view = getViewVector(f2);

	Vec3 limit = pos + view * f1;
	return m_pLevel->clip(pos, limit);
}

void Mob::travel(const Vec2& pos)
{
	float x1, x2, dragFactor, oldYPos = m_pos.y;
	if (isInWater())
	{
		moveRelative(Vec3(pos.x, 0.02f, pos.y));
		move(m_vel);
		x1 = 0.8f;
		goto label_3;
	}
	if (isInLava())
	{
		moveRelative(Vec3(pos.x, 0.02f, pos.y));
		move(m_vel);
		x1 = 0.5f;
	label_3:

		m_vel.y = m_vel.y * x1 - 0.02f;
		m_vel.x *= x1;
		m_vel.z *= x1;

		if (m_bHorizontalCollision)
		{
			if (isFree(Vec3(m_vel.x, m_vel.y + 0.6f - m_pos.y + oldYPos, m_vel.z)))
				m_vel.y = 0.3f;
		}

		return;
	}

	if (!m_onGround)
	{
		x2 = 0.02f;
	}
	else
	{
		float _x1;
		TilePos tilePos(m_pos.x, m_hitbox.min.y, m_pos.z);;
		tilePos.y -= 1;
		TileID tile = m_pLevel->getTile(tilePos);
		if (tile <= 0)
			_x1 = 0.546f;
		else
			_x1 = Tile::tiles[tile]->field_30 * 0.91f;

		assert(_x1 != 0.0f);

		x2 = (0.16277f / (_x1 * _x1 * _x1)) * 0.1f;
	}

	moveRelative(Vec3(pos.x, x2, pos.y));

	if (!m_onGround)
	{
		dragFactor = 0.91f;
	}
	else
	{
		//@HUH: repeated code. Could be an inlined function?
		TilePos tilePos = TilePos(m_pos);
		tilePos.y -= 1;
		TileID tile = m_pLevel->getTile(tilePos);
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

	move(m_vel);

	if (m_bHorizontalCollision && onLadder())
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

bool Mob::canSee(Entity* pEnt) const
{
	Vec3 v1 = m_pos;
	v1.y += getHeadHeight();
	Vec3 v2 = pEnt->m_pos;
	v2.y += pEnt->getHeadHeight();

	return m_pLevel->clip(v1, v2).m_hitType == HitResult::NONE;
}

void Mob::updateWalkAnim()
{
	field_128 = m_walkAnimSpeed;

	float diffX = m_pos.x - m_oPos.x;
	float diffZ = m_pos.z - m_oPos.z;

	float spd = 4.0f * Mth::sqrt(diffX * diffX + diffZ * diffZ);
	if (spd > 1.0f)
		spd = 1.0f;

	m_walkAnimSpeed += (spd - m_walkAnimSpeed) * 0.4f;
	field_130 += m_walkAnimSpeed;
}

void Mob::aiStep()
{
	if (isImmobile())
	{
		m_bJumping = 0;
		field_B00 = Vec2::ZERO;
	}
	else if (!field_F0)
	{
		updateAi();
	}

	bool bIsInWater = isInWater(), bIsInLava = isInLava();
	if (m_bJumping)
	{
		if (bIsInWater || bIsInLava)
			m_vel.y += 0.04f;
		else if (m_onGround)
			jumpFromGround();
	}

	field_B00.x *= 0.98f;
	field_B00.y *= 0.98f;
	field_B08 *= 0.9f;

	travel(field_B00);

	AABB aabb = m_hitbox;
	aabb.grow(0.2f, 0.2f, 0.2f);

	EntityVector ents = m_pLevel->getEntities(this, aabb);
	for (EntityVector::iterator it = ents.begin(); it != ents.end(); it++)
	{
		Entity* pEnt = *it;
		if (pEnt->isPushable())
			pEnt->push(this);
	}
}

void Mob::lookAt(Entity* pEnt, float a3, float a4)
{
	float diffX = pEnt->m_pos.x - m_pos.x;
	float diffZ = pEnt->m_pos.z - m_pos.z;

	float q1 = (pEnt->m_hitbox.min.y + pEnt->m_hitbox.max.y) / 2 - (m_pos.y + getHeadHeight());
	float p1 = Mth::sqrt(diffX * diffX + diffZ * diffZ);

	float x1 = atan2f(diffZ, diffX);
	float x2 = atan2f(q1, p1);

	setRot(Vec2(-rotlerp(m_rot.y, x2 * 180.0f / float(M_PI), a4),
	              rotlerp(m_rot.x, x1 * 180.0f / float(M_PI) - 90.0f, a3)));
}

bool Mob::canSpawn() const
{
	return m_pLevel->getCubes(this, m_hitbox)->empty();
}

float Mob::getAttackAnim(float f) const
{
	float x = m_attackAnim - m_oAttackAnim;
	
	if (x < 0.0f)
		x += 1.0f;

	return m_oAttackAnim + (x * f);
}

Vec3 Mob::getPos(float f) const
{
	if (f == 1.0f)
		return m_pos;

	return Vec3(
        Mth::Lerp(m_oPos.x, m_pos.x, f),
		Mth::Lerp(m_oPos.y, m_pos.y, f),
		Mth::Lerp(m_oPos.z, m_pos.z, f)
	);
}

Vec3 Mob::getViewVector(float f) const
{
	constexpr float C_180_OVER_PI = 0.017453f;
	constexpr float C_PI = 3.1416f;
	
	if (f == 1.0)
	{
		Vec3 x(Mth::cos(-(m_rot.x * C_180_OVER_PI) - C_PI),
			   Mth::sin(-(m_rot.x * C_180_OVER_PI) - C_PI),
			   -Mth::cos(-(m_rot.y * C_180_OVER_PI)));

		return Vec3(x.x * x.z, Mth::sin(-(m_rot.y * C_180_OVER_PI)), x.y * x.z);
	}

	float x1 = m_rotPrev.y + (m_rot.y - m_rotPrev.y) * f;
	float x2 = -((m_rotPrev.x + (m_rot.x - m_rotPrev.x) * f) * C_180_OVER_PI) - C_PI;
	float x3 = Mth::cos(x2);
	float x4 = Mth::sin(x2);
	float x5 = -(x1 * C_180_OVER_PI);
	float x6 = -Mth::cos(x5);

	return Vec3(x4 * x6, Mth::sin(x5), x3 * x6);
}

void Mob::actuallyHurt(int damage)
{
	m_health -= damage;
}

void Mob::dropDeathLoot()
{
	int itemId = getDeathLoot();
	if (itemId > 0)
	{
		int desiredItemCount = m_random.nextInt(3);
		for (int itemCount = 0; itemCount < desiredItemCount; itemCount++)
		{
			spawnAtLocation(itemId, 1);
		}
	}
}

void Mob::jumpFromGround()
{
	m_vel.y = 0.42f;
}

void Mob::updateAi()
{
	field_AFC++;

	checkDespawn();

	field_B00 = Vec2::ZERO;

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

		m_rot.x += field_B08;
		m_rot.y = field_B10;
	}

	if (isInWater() || isInLava())
	{
		m_bJumping = m_random.nextFloat() < 0.8f;
	}
}

void Mob::checkDespawn(Mob* nearestMob)
{
	// if we need to remove ourselves when far away, and there's a player around
	// (if there's no players, we're on a headless server)
	if (nearestMob)
	{
		float distSqr = nearestMob->distanceToSqr_inline(m_pos);
		bool remWhenFar = removeWhenFarAway();
		if (remWhenFar && distSqr > 9216.0f)
			remove();

		if (field_AFC <= 600)
			field_AFC = 0;
		else if (m_random.nextInt(800) == 0 && remWhenFar && distSqr >= 1024.0f)
			remove();
		else
			field_AFC = 0;
	}
}

void Mob::checkDespawn()
{
	Mob* nearestPlayer = m_pLevel->getNearestPlayer(this, -1.0f);
	checkDespawn(nearestPlayer);
}

void Mob::swing()
{
	// Found this very useful check in 0.7.0
	if (!m_bSwinging || (m_swingTime >= 6 / 2 || m_swingTime < 0))
	{
		m_swingTime = -1;
		m_bSwinging = true;
	}
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

void Mob::updateAttackAnim()
{
	if (m_bSwinging)
	{
		m_swingTime++;
		if (m_swingTime > 7)
		{
			m_swingTime = 0;
			m_bSwinging = false;
		}
	}
	else
	{
		m_swingTime = 0;
	}

	m_attackAnim = m_swingTime * 0.125f;
}