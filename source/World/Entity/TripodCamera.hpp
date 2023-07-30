#pragma once

#include "Mob.hpp"

class Level;
class Player;

class TripodCamera : public Mob
{
public:
	TripodCamera(Level*, Player*, float, float, float);

	void defineSynchedData() override;
	float getShadowHeightOffs() override;
	bool interact(Player* player) override;
	int interactPreventDefault() override;
	bool isPickable() override;
	bool isPushable() override;
	void tick() override;

public:
	int field_B8C = 0;
	int field_B90 = 80;
	Player* m_owner = nullptr;
	bool m_bActive = false;
};

