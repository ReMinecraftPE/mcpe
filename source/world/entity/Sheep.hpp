#pragma once

#include "Animal.hpp"

class Sheep : public Animal
{
public:
	static const float COLOR[][3];

public:
	Sheep(Level* pLevel);

private:
	void _defineEntityData();

public:
	std::string getAmbientSound() const override { return "mob.sheep"; };
	std::string getDeathSound() const override { return "mob.sheep"; }
	std::string getHurtSound() const override { return "mob.sheep"; }
	int getEntityTypeId() const override { return ENTITY_TYPE_SHEEP; }
	virtual bool hurt(Entity*, int) override;
	//TODO: addAdditonalSaveData
	//TODO: readAdditionalSaveData

	Entity* getBreedOffspring(Animal* pOther) { return new Sheep(m_pLevel); }

    int getColor() const;
    void setColor(int);
    bool isSheared() const;
    void setSheared(bool);

	static int getSheepColor(Random&);
};
