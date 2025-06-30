#pragma once

#include "world/entity/Monster.hpp"
#include "world/entity/Arrow.hpp"

class Skeleton : public Monster
{
private:
	static ItemInstance* bow;

public:
	Skeleton(Level* pLevel);

	std::string getAmbientSound() const override { return "mob.skeleton"; }
	std::string getDeathSound() const override { return "mob.skeletonhurt"; }
	std::string getHurtSound() const override { return "mob.skeletonhurt"; }
	
	void aiStep() override;
	
	void checkHurtTarget(Entity* ent, float f) override;
	
	int getDeathLoot() const override { return ITEM_ARROW; }
	
	void dropDeathLoot() override;
	
	ItemInstance* getCarriedItem() override;
};