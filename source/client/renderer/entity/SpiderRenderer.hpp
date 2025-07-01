#pragma once

#include "MobRenderer.hpp"

class SpiderRenderer : public MobRenderer
{
public:
	SpiderRenderer();
	~SpiderRenderer();

	float getFlipDegrees(Mob*) override { return 180.0f; }
	int prepareArmor(Mob* spider, int layer, float a) override;
};
