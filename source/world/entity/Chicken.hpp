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

	std::string getAmbientSound() const override { return "mob.chicken"; }
	std::string getDeathSound() const override { return "mob.chickenhurt"; }
	std::string getHurtSound() const override { return "mob.chickenhurt"; }
	int getMaxHealth() const override { return 4; }
	void aiStep() override;
	void dropDeathLoot() override;
	void causeFallDamage(float) override { return; }
	void addAdditionalSaveData(CompoundTag& tag) const override;
	void readAdditionalSaveData(const CompoundTag& tag) override;

	Entity* getBreedOffspring(Animal* pOther) { return new Chicken(m_pLevel); }
};
