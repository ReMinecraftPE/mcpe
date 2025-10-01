/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "Chicken.hpp"
#include "world/level/Level.hpp"

Chicken::Chicken(Level* pLevel) : Animal(pLevel)
{
	m_pDescriptor = &EntityTypeDescriptor::chicken;
	field_C8 = RENDER_CHICKEN;
	m_texture = "mob/chicken.png";
	setSize(0.3f, 0.7f);

	m_flap = m_oFlap = m_flapSpeed = m_oFlapSpeed = m_flapping = 0.0f;
	m_eggTime = m_random.nextInt(6000) + 6000;
}

void Chicken::aiStep()
{
	Animal::aiStep();

	m_oFlap = m_flap;
	m_oFlapSpeed = m_flapSpeed;
	m_flapSpeed += (m_bOnGround ? -1.0f : 4.0f) * 0.3f;
	if (m_flapSpeed >= 0.0f)
	{
		if (m_flapSpeed > 1.0f)
			m_flapSpeed = 1.0f;
	}
	else
	{
		m_flapSpeed = 0.0f;
	}
	if (!m_bOnGround && m_flapping < 1.0f)
		m_flapping = 1.0f;

	m_flapping *= 0.9f;
	if (!m_bOnGround && m_vel.y < 0.0f)
		m_vel.y *= 0.6f;

	m_flap += m_flapping * 2.0f;
	if (!m_pLevel->m_bIsClientSide && isAlive() && !isBaby() /* && !isChickenJockey()*/ && --m_eggTime <= 0)
	{
		m_pLevel->playSound(this, "mob.chickenplop", 1.0f, (m_random.nextFloat() - m_random.nextFloat()) * 0.2f + 1.0f);
		spawnAtLocation(Item::egg->m_itemID, 1);
		m_eggTime = m_random.nextInt(6000) + 6000;
	}
}

void Chicken::dropDeathLoot()
{

	int desiredItemCount = m_random.nextInt(3);
	for (int itemCount = 0; itemCount < desiredItemCount; itemCount++)
	{
		spawnAtLocation(Item::feather->m_itemID, 1);
	}

	/*if (isOnFire())
		spawnAtLocation(Item::chicken_cooked->m_itemID, 1);
	else
		spawnAtLocation(Item::chicken_raw->m_itemID, 1);*/
}

void Chicken::addAdditionalSaveData(CompoundTag& tag) const
{
	Animal::addAdditionalSaveData(tag);
}

void Chicken::readAdditionalSaveData(const CompoundTag& tag)
{
	Animal::readAdditionalSaveData(tag);
}