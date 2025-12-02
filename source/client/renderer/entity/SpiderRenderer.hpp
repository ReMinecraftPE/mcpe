#pragma once

#include "MobRenderer.hpp"

class SpiderRenderer : public MobRenderer
{
public:
	SpiderRenderer();
	~SpiderRenderer();

	float getFlipDegrees(const Mob& mob) override { return 180.0f; }
	int prepareArmor(const Mob& spider, int layer, float a) override;
};
