#pragma once

#include "world/entity/Monster.hpp"

#define DATA_SWELL_DIR (16)
#define MAX_SWELL (30)

class Creeper : public Monster
{
public:
	Creeper(Level* pLevel);

	void tick() override;
	std::string getHurtSound() const override { return "mob.creeper"; }
	std::string getDeathSound() const override { return "mob.creeperdeath"; }
	float getSwelling(float f) const;
	int getSwellDir() const { return m_entityData.get<int>(DATA_SWELL_DIR); }
	void setSwellDir(int value) { m_entityData.set<int>(DATA_SWELL_DIR, value); }
	int getDeathLoot() const override { return ITEM_SULPHUR; }

	virtual void die(Entity* pCulprit) override;
	virtual void checkHurtTarget(Entity*, float) override;

private:
	void _defineEntityData();

public:
	int m_swell;
	int m_oldSwell;
};