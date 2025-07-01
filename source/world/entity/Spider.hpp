#pragma once

#include "world/entity/Monster.hpp"

class Spider : public Monster
{
public:
	Spider(Level* pLevel);

	std::string getAmbientSound() const override { return "mob.spider"; }
	std::string getDeathSound() const override { return "mob.spiderdeath"; }
	std::string getHurtSound() const override { return "mob.spider"; }
	int getDeathLoot() const override { return ITEM_STRING; }
	float getRideHeight() const { return m_bbHeight * 0.75f - 0.5f; }

	Entity* findAttackTarget() override;
	void checkHurtTarget(Entity* ent, float var2) override;
	
	bool onLadder() const override { return m_bHorizontalCollision; }
};