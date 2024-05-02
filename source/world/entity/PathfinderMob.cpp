/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "PathfinderMob.hpp"
#include "world/level/Level.hpp"

PathfinderMob::PathfinderMob(Level* pLevel) : Mob(pLevel)
{
	field_BA0 = false;
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

bool PathfinderMob::checkHurtTarget(Entity* pEnt, float f)
{
	return false;
}

void PathfinderMob::checkCantSeeTarget(Entity* pEnt, float f)
{
	if (f > 32.0f)
		setAttackTarget(nullptr);
}

float PathfinderMob::getWalkTargetValue(int, int, int)
{
	return 0.0f;
}

bool PathfinderMob::shouldHoldGround()
{
	return false;
}

void PathfinderMob::findRandomStrollLocation()
{
	bool foundLocation = false;

	float maxDist = -99999.0f;
	int dx = -1, dy = -1, dz = -1;

	for (int i = 0; i < 10; i++)
	{
		int testX = Mth::floor(float(m_pos.x + m_random.nextInt(13)) - 6.0f);
		int testY = Mth::floor(float(m_pos.y + m_random.nextInt(7))  - 3.0f);
		int testZ = Mth::floor(float(m_pos.z + m_random.nextInt(13)) - 6.0f);

		float wtv = getWalkTargetValue(testX, testY, testZ);
		if (maxDist < wtv)
		{
			maxDist = wtv;
			dx = testX;
			dy = testY;
			dz = testZ;
			foundLocation = true;
		}
	}

	if (foundLocation)
	{
		m_pLevel->findPath(&m_path, this, dx, dy, dz, 10.0f);
	}
}

float PathfinderMob::getWalkingSpeedModifier()
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

	return getWalkTargetValue(Mth::floor(m_pos.x), Mth::floor(m_hitbox.min.y), Mth::floor(m_pos.z)) >= 0.0f;
}

static Vec3 GetNodePosition(Node* pNode, Entity* pEnt)
{
	float offset = float(int(pEnt->field_88 + 1.0f)) * 0.5f;
	float fx = float(pNode->m_x) + offset;
	float fz = float(pNode->m_z) + offset;
	float fy = float(pNode->m_y);
	return Vec3(fx, fy, fz);
}

void PathfinderMob::updateAi()
{
	if (field_BA4 > 0)
		field_BA4--;

	field_BA0 = shouldHoldGround();

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

	if (!field_BA0 && m_pAttackTarget && (m_path.empty() || m_random.nextInt(20) != 0))
	{
		m_pLevel->findPath(&m_path, this, m_pAttackTarget, 16.0f);
	}
	else if (!field_BA0 && ((m_path.empty() && m_random.nextInt(180) == 0) || field_BA4 > 0 || m_random.nextInt(120) == 0))
	{
		if (field_AFC < 100)
			findRandomStrollLocation();
	}


	m_pitch = 0.0f;

	if (m_path.empty() || m_random.nextInt(100) == 0)
	{
		Mob::updateAi();
		return;
	}

	// follow path
	Node* pCurrNode = m_path.getCurrentNode();
	Vec3 nodePos = GetNodePosition(pCurrNode, this);

	// Skip all nodes that we are inside of.
	float sqdbl = (field_88 * 2.0f) * (field_88 * 2.0f);
	while (sqdbl > nodePos.distanceToSqr(Vec3(m_pos.x, nodePos.y, m_pos.z)))
	{
		m_path.advance();

		if (m_path.endOfPath())
		{
			m_path.clear();
			pCurrNode = nullptr;
			break;
		}

		pCurrNode = m_path.getCurrentNode();
		nodePos = GetNodePosition(pCurrNode, this);
	}

	field_B0C = false;

	bool inWater = isInWater();
	bool inLava = isInLava();

	if (pCurrNode)
	{
		float ang = Mth::atan2(nodePos.z - m_pos.z, nodePos.x - m_pos.x) * 180.0f / float(M_PI) - 90.0f;
		float heightDiff = nodePos.y - Mth::floor(m_hitbox.min.y + 0.5f);

		field_B04 = field_B14;

		float angDiff = ang - m_yaw;
		while (angDiff < -180.0f) angDiff += 360.0f;
		while (angDiff >= 180.0f) angDiff -= 360.0f;

		if (angDiff > +30.0f) angDiff = +30.0f;
		if (angDiff < -30.0f) angDiff = -30.0f;

		float oldYaw = m_yaw;

		m_yaw += angDiff;

		if (field_BA0 && m_pAttackTarget)
		{
			float ang2 = Mth::atan2(m_pAttackTarget->m_pos.z - m_pos.z, m_pAttackTarget->m_pos.x - m_pos.x) * 180.0f / float(M_PI) - 90.0f;
			m_yaw = ang2;

			float thing = ((((angDiff + oldYaw) - ang2) + 90.0f) * float(M_PI)) / 180.0f;

			// @NOTE: Using old field_B04 value
			field_B00 = -field_B04 * Mth::sin(thing);
			field_B04 =  field_B04 * Mth::cos(thing);
		}

		if (heightDiff > 0.0f)
			field_B0C = true;
	}

	if (m_pAttackTarget)
		lookAt(m_pAttackTarget, 30.0f, 30.0f);

	// if we hit a wall while moving
	if (field_7D && !isPathFinding())
		field_B0C = true;

	// if we're in water, try to swim up
	if (m_random.nextFloat() < 0.8f && (inWater || inLava))
		field_B0C = true;
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
