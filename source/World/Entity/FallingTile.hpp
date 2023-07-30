#pragma once

#include "Entity.hpp"

class FallingTile : public Entity
{
public:
	FallingTile(Level*);
	FallingTile(Level*, float x, float y, float z, int id);

	float getShadowHeightOffs() override;
	bool isPickable() override;
	void tick() override;

	Level* getLevel();

public:
	int m_id;
	int field_E0 = 0;
};

