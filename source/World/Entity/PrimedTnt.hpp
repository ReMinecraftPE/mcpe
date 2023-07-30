#pragma once

#include "Entity.hpp"

class PrimedTnt : public Entity
{
public:
	PrimedTnt(Level*);
	PrimedTnt(Level*, float, float, float);

	float getShadowHeightOffs() override;
	bool isPickable() override;
	void tick() override;

	void explode();

public:
	int m_fuseTimer;
};

