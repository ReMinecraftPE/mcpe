/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "PathfinderMob.hpp"

class Animal : public PathfinderMob
{
public:
	Animal(Level* pLevel);
	// TODO: void addAdditonalSaveData(CompoundTag*) override;
	// TODO: void readAdditonalSaveData(CompoundTag*) override;
	void aiStep() override;
	bool isBaby() override;
	bool canSpawn() override;
	Entity* findAttackTarget() override;
	int getAmbientSoundInterval() override;
	int getCreatureBaseType() override;
	float getWalkTargetValue(int x, int y, int z) override;
	bool hurt(Entity* pCulprit, int damage) override;
	bool removeWhenFarAway() override;

	int getAge();
	void setAge(int age);

private:
	int field_BB4;
	int m_age;
};
