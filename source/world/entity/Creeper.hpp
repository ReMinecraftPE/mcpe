#pragma once

#include "world/entity/Monster.hpp"

class Creeper : public Monster
{
public:
	Creeper(Level* pLevel);

	void tick() override;
	std::string getHurtSound() const override { return "mob.creeper"; }
	std::string getDeathSound() const override { return "mob.creeperdeath"; }
	float getSwelling(float f);
	int getSwellDir() { return m_swellDir; }
	void setSwellDir(int value) { m_swellDir = value; }
	int getDeathLoot() const override { return ITEM_SULPHUR; }

	virtual void die(Entity* pCulprit) override;
	virtual void checkHurtTarget(Entity*, float) override;

public:
	int m_swell;
	int m_oldSwell;
	int m_swellDir;
	int m_maxSwell;
};