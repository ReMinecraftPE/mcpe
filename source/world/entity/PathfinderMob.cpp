/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "PathfinderMob.hpp"
#include "world/level/Level.hpp"

#define MAX_TURN (30.0f)

PathfinderMob::PathfinderMob(Level* pLevel) : Mob(pLevel)
{
	m_pAttackTarget = nullptr;
	m_bHoldGround = false;
	field_BA4 = 0;
}

Entity* PathfinderMob::getAttackTarget()
{
	return m_pAttackTarget;
}

void PathfinderMob::setAttackTarget(Entity* pEnt)
{
	m_pAttackTarget = pEnt;
}

Entity* PathfinderMob::findAttackTarget()
{
	return nullptr;
}

void PathfinderMob::checkHurtTarget(Entity* pEnt, float f)
{
	// Override this function in your own mob.
}

void PathfinderMob::checkCantSeeTarget(Entity* pEnt, float f)
{
	if (f > 32.0f)
		setAttackTarget(nullptr);
}

float PathfinderMob::getWalkTargetValue(const TilePos& pos) const
{
	return 0.0f;
}

bool PathfinderMob::shouldHoldGround() const
{
	return false;
}

void PathfinderMob::findRandomStrollLocation()
{
	bool foundLocation = false;

	float maxDist = -99999.0f;
	TilePos pos(-1, -1, -1);

	for (int i = 0; i < 10; i++)
	{
		TilePos test(float(m_pos.x + m_random.nextInt(13)) - 6.0f,
					 float(m_pos.y + m_random.nextInt(7))  - 3.0f,
					 float(m_pos.z + m_random.nextInt(13)) - 6.0f);

		float wtv = getWalkTargetValue(test);
		if (maxDist < wtv)
		{
			maxDist = wtv;
			pos = test;
			foundLocation = true;
		}
	}

	if (foundLocation)
	{
		m_pLevel->findPath(&m_path, this, pos, 10.0f);
	}
}

float PathfinderMob::getWalkingSpeedModifier() const
{
	float mod = Mob::getWalkingSpeedModifier();

	if (field_BA4 > 0)
		mod *= 2.0f;

	return mod;
}

bool PathfinderMob::canSpawn()
{
	if (!Mob::canSpawn())
		return false;

	return getWalkTargetValue(TilePos(m_pos.x, m_hitbox.min.y, m_pos.z)) >= 0.0f;
}

void PathfinderMob::updateAi()
{
	if (field_BA4 > 0)
		field_BA4--;

	m_bHoldGround = shouldHoldGround();

	if (m_pAttackTarget)
	{
		if (!m_pAttackTarget->isAlive())
		{
			m_pAttackTarget = nullptr;
		}
		else
		{
			float dist = m_pAttackTarget->distanceTo(this);

			if (canSee(m_pAttackTarget))
				checkHurtTarget(m_pAttackTarget, dist);
			else
				checkCantSeeTarget(m_pAttackTarget, dist);
		}
	}
	else
	{
		m_pAttackTarget = findAttackTarget();
		if (m_pAttackTarget)
			m_pLevel->findPath(&m_path, this, m_pAttackTarget, 16.0f);
	}

	if (!m_bHoldGround && m_pAttackTarget && (m_path.empty() || m_random.nextInt(20) != 0))
	{
		m_pLevel->findPath(&m_path, this, m_pAttackTarget, 16.0f);
	}
	else if (!m_bHoldGround && ((m_path.empty() && m_random.nextInt(180) == 0) || field_BA4 > 0 || m_random.nextInt(120) == 0))
	{
		if (m_noActionTime < 100)
			findRandomStrollLocation();
	}


	m_rot.y = 0.0f;

	if (m_path.empty() || m_random.nextInt(100) == 0)
	{
		Mob::updateAi();
		return;
	}

	// follow path
	Node* pCurrNode = m_path.getCurrentNode();
	Vec3 nodePos = m_path.currentPos(*this);

	// Skip all nodes that we are inside of.
	float sqdbl = (m_bbWidth * 2.0f) * (m_bbWidth * 2.0f);
	while (sqdbl > nodePos.distanceToSqr(Vec3(m_pos.x, nodePos.y, m_pos.z)))
	{
		m_path.next();

		if (m_path.isDone())
		{
			m_path.clear();
			pCurrNode = nullptr;
			break;
		}

		pCurrNode = m_path.getCurrentNode();
		nodePos = m_path.currentPos(*this);
	}

	m_bJumping = false;

	bool inWater = isInWater();
	bool inLava = isInLava();

	if (pCurrNode)
	{
		float ang = Mth::atan2(nodePos.z - m_pos.z, nodePos.x - m_pos.x) * 180.0f / float(M_PI) - 90.0f;
		float heightDiff = nodePos.y - Mth::floor(m_hitbox.min.y + 0.5f ); // +0.5f is not present on b1.2_02, but is present on 0.12.1

		field_B00.y = m_runSpeed;

		float angDiff = ang - m_rot.x;
		while (angDiff < -180.0f) angDiff += 360.0f;
		while (angDiff >= 180.0f) angDiff -= 360.0f;

		if (angDiff > +MAX_TURN) angDiff = +MAX_TURN;
		if (angDiff < -MAX_TURN) angDiff = -MAX_TURN;

		float oldYaw = m_rot.x;

		m_rot.x += angDiff;

		if (m_bHoldGround && m_pAttackTarget)
		{
			float ang2 = Mth::atan2(m_pAttackTarget->m_pos.z - m_pos.z, m_pAttackTarget->m_pos.x - m_pos.x) * 180.0f / float(M_PI) - 90.0f;
			m_rot.x = ang2;

			float thing = ((((angDiff + oldYaw) - ang2) + 90.0f) * float(M_PI)) / 180.0f;

			// @NOTE: Using old field_B00.y value. This is intentional and consistent with b1.2_02.
			field_B00.x = -field_B00.y * Mth::sin(thing);
			field_B00.y =  field_B00.y * Mth::cos(thing);
		}

		if (heightDiff > 0.0f)
			m_bJumping = true;
	}

	if (m_pAttackTarget)
		lookAt(m_pAttackTarget, MAX_TURN, MAX_TURN);

	// if we hit a wall while moving
	if (m_bHorizontalCollision && isPathFinding())
		m_bJumping = true;

	// if we're in water, try to swim up
	if (m_random.nextFloat() < 0.8f && (inWater || inLava))
		m_bJumping = true;
}

void PathfinderMob::setPath(Path& path)
{
	// @TODO: Not sure if it calls the destructor here. In original Minecraft, it doesn't.
	m_path = path;
}

bool PathfinderMob::isPathFinding()
{
	return !m_path.empty();
}
