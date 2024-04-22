/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "Animal.hpp"

class Chicken : public Animal
{
public:
	float m_flap;
	float m_oFlap;
	float m_flapSpeed;
	float m_oFlapSpeed;
	float m_flapping;
	int m_eggTime;

public:
	Chicken(Level* pLevel);

	std::string getAmbientSound() override;
	std::string getDeathSound() override;
	std::string getHurtSound() override;
	void dropDeathLoot() override;
	int getEntityTypeId() override;
	int getMaxHealth() override;
	void aiStep() override;
	void causeFallDamage(float) override;

	Entity* getBreedOffspring(Animal* pOther);
};
