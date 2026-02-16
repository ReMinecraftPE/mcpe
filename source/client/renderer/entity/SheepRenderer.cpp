#include "SheepRenderer.hpp"
#include "renderer/ShaderConstants.hpp"

SheepRenderer::SheepRenderer(Model* pModel, Model* pArmor, float f) : MobRenderer(pModel, f)
{
	setArmor(pArmor);
}

SheepRenderer::~SheepRenderer()
{
}

int SheepRenderer::prepareArmor(const Mob& mob, int layer, float a)
{
    const Sheep& sheep = (const Sheep&)mob;
    if (layer == 0 && !sheep.isSheared())
    {
        bindTexture("mob/sheep_fur.png");
        float brightness = sheep.getBrightness(a);
        int color = sheep.getColor();
        currentShaderColor = Sheep::COLOR[color];
        currentShaderDarkColor = Color(brightness, brightness, brightness);
        return 1;
    }
    else
    {
        return 0;
    }
}

