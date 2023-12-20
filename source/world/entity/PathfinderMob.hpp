/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "Mob.hpp"
#include "world/level/path/Path.hpp"

class PathfinderMob : public Mob
{
public:
	PathfinderMob(Level* pLevel);

	virtual Entity* getAttackTarget();
	virtual void setAttackTarget(Entity*);
	virtual Entity* findAttackTarget();
	virtual bool checkHurtTarget(Entity*, float);
	virtual void checkCantSeeTarget(Entity*, float);
	virtual float getWalkTargetValue(int, int, int);
	virtual bool shouldHoldGround();
	virtual void findRandomStrollLocation();

	float getWalkingSpeedModifier() override;
	bool canSpawn() override;
	void updateAi() override;

	void setPath(Path& path);
	bool isPathFinding();

protected:
	friend class Animal;
	Entity* m_pAttackTarget;
	bool field_BA0;
	int field_BA4;
	Path m_path;
};
