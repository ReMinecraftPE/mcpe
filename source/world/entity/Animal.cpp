/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "Animal.hpp"
#include "world/level/Level.hpp"

Animal::Animal(Level* pLevel) : PathfinderMob(pLevel)
{
	field_BB4 = 0;
	m_age = 0;
}

void Animal::aiStep()
{
	int age = getAge();
	if (age < 0)
		setAge(age + 1);
	else if (age)
		setAge(age - 1);

	Mob::aiStep();
}

bool Animal::isBaby() const
{
	return getAge() < 0;
}

bool Animal::canSpawn() const
{
	int x = Mth::floor(m_pos.x);
	int y = Mth::floor(m_hitbox.min.y);
	int z = Mth::floor(m_pos.z);

	if (m_pLevel->getTile(x, y - 1, z) != Tile::grass->m_ID || m_pLevel->getRawBrightness(x, y, z) < 8)
		return false;

	return PathfinderMob::canSpawn();
}

Entity* Animal::findAttackTarget()
{
	// Animals are passive mobs.
	return nullptr;
}

int Animal::getAmbientSoundInterval() const
{
	return 240;
}

int Animal::getCreatureBaseType() const
{
	return BASE_ANIMAL;
}

float Animal::getWalkTargetValue(int x, int y, int z) const
{
	// Animals would rather walk on grass.
	if (m_pLevel->getTile(x, y - 1, z) == Tile::grass->m_ID)
		return 10.0f;

	// Animals will avoid dark areas.
	return m_pLevel->getBrightness(x, y, z) - 0.5f;
}

bool Animal::hurt(Entity* pCulprit, int damage)
{
	// Run around erratically for three seconds.
	field_BA4 = 60;

	m_pAttackTarget = nullptr;
	field_BB4 = 0;

	return Mob::hurt(pCulprit, damage);
}

bool Animal::removeWhenFarAway() const
{
	return false;
}

int Animal::getAge() const
{
	return m_age;
}

void Animal::setAge(int age)
{
	m_age = age;
}
