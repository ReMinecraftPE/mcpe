/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "Animal.hpp"

class Pig : public Animal
{
public:
	Pig(Level* pLevel);

	std::string getAmbientSound() override;
	std::string getDeathSound() override;
	std::string getHurtSound() override;
	int getDeathLoot() override;
	int getEntityTypeId() override;
	int getMaxHealth() override;
	bool interact(Player*) override;

	bool hasSaddle();
	void setSaddle(bool b);
	Entity* getBreedOffspring(Animal* pOther);
};
