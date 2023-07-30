#pragma once

#include "Cube.hpp"
#include "Mob.hpp"

class Model
{
public:
	virtual void onGraphicsReset();
	virtual void prepareMobModel(Mob*, float, float, float);
	virtual void render(float, float, float, float, float, float);
	virtual void renderHorrible(float, float, float, float, float, float);
	virtual void setupAnim(float, float, float, float, float, float);

public:
	float field_4 = 0.0f;
	bool field_8 = false;
};
