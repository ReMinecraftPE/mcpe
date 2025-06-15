#include "SpiderRenderer.hpp"
#include "client/model/SpiderModel.hpp"

SpiderRenderer::SpiderRenderer() : MobRenderer(new SpiderModel, 1.0f)
{
	setArmor(new SpiderModel);
}

SpiderRenderer::~SpiderRenderer()
{
}