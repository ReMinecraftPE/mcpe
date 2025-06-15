#pragma once

#include "MobRenderer.hpp"
#include "world/entity/Spider.hpp"

class SpiderRenderer : public MobRenderer
{
public:
	SpiderRenderer();
	~SpiderRenderer();

	float getFlipDegrees(Mob*) override { return 180.0f; }
	int prepareArmor(Mob* spider, int layer, float a) override {
		if (layer != 0)
			return 0;

		bindTexture("mob/spider_eyes.png");
		float br = (1.0f - reinterpret_cast<Spider*>(spider)->getBrightness(1.0f)) * 0.5f;
		glEnable(3042);
		glDisable(3008);
		glBlendFunc(770, 771);
		glColor4f(1.0f, 1.0f, 1.0f, br);
		return 1;
	}
};
