/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "Animal.hpp"

class Cow : public Animal
{
public:
	Cow(Level* pLevel);

	std::string getAmbientSound() const override { return "mob.cow"; };
	std::string getDeathSound() const override { return "mob.cowhurt"; }
	std::string getHurtSound() const override { return "mob.cowhurt"; }
	int getDeathLoot() const override { return ITEM_LEATHER; }
	int getMaxHealth() const override { return 10; }
	float getSoundVolume() const override { return 0.4f; }
	void addAdditionalSaveData(CompoundTag& tag) const override;
	void readAdditionalSaveData(const CompoundTag& tag) override;

	Entity* getBreedOffspring(Animal* pOther) { return new Cow(m_pLevel); }
};
