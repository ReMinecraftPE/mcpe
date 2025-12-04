#include "SpiderRenderer.hpp"
#include "client/model/models/SpiderModel.hpp"
#include "renderer/ShaderConstants.hpp"
#include "world/entity/Mob.hpp"

SpiderRenderer::SpiderRenderer() : MobRenderer(new SpiderModel, 1.0f)
{
	setArmor(new SpiderModel);
}

SpiderRenderer::~SpiderRenderer()
{
}

int SpiderRenderer::prepareArmor(const Mob& spider, int layer, float a)
{
	if (layer != 0)
		return 0;

	if (!bindTexture("mob/spider_eyes.png", false))
		return 0;

	float br = (1.0f - spider.getBrightness(1.0f)) * 0.5f;
	currentShaderColor = Color::WHITE;
	currentShaderDarkColor = Color(1.0f, 1.0f, 1.0f, br);

	return 1;
}