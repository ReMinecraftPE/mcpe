#include "world/entity/PathfinderMob.hpp"
#include "world/level/Level.hpp"

class Monster : public PathfinderMob
{
public:
	Monster(Level* pLevel);

	void aiStep() override;
	void tick() override;
	Entity* findAttackTarget() override;
	bool hurt(Entity* pCulprit, int damage) override;
	virtual void checkHurtTarget(Entity*, float) override;
	virtual float getWalkTargetValue(const TilePos& pos) const override;
	bool canSpawn() override;

protected:
	int m_attackDamage;
};