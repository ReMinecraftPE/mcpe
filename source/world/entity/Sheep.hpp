#pragma once

#include "Animal.hpp"

class Sheep : public Animal
{
public:
	static const float COLOR[][3];
	static const unsigned int COLOR_COUNT; // NumColors on PE, stupid name

public:
	Sheep(Level* pLevel);

private:
	void _defineEntityData();

public:
	std::string getAmbientSound() const override { return "mob.sheep"; };
	std::string getDeathSound() const override { return "mob.sheep"; }
	std::string getHurtSound() const override { return "mob.sheep"; }
	virtual bool hurt(Entity*, int) override;
	void addAdditionalSaveData(CompoundTag& tag) const override;
	void readAdditionalSaveData(const CompoundTag& tag) override;

	Entity* getBreedOffspring(Animal* pOther) { return new Sheep(m_pLevel); }

    int getColor() const;
    void setColor(int);
    bool isSheared() const;
    void setSheared(bool);

	static int getSheepColor(Random&);
};
