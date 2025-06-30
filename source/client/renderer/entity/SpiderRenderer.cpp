#include "SpiderRenderer.hpp"
#include "world/entity/Mob.hpp"
#include "client/model/SpiderModel.hpp"

SpiderRenderer::SpiderRenderer() : MobRenderer(new SpiderModel, 1.0f)
{
	setArmor(new SpiderModel);
}

SpiderRenderer::~SpiderRenderer()
{
}

int SpiderRenderer::prepareArmor(Mob* spider, int layer, float a)
{
	if (layer != 0)
		return 0;

	if (!bindTexture("mob/spider_eyes.png", false))
		return 0;

	float br = (1.0f - spider->getBrightness(1.0f)) * 0.5f;
	glEnable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, br);

	return 1;
}