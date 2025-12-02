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
        bindTexture("/mob/sheep_fur.png");
        float brightness = sheep.getBrightness(a);
        int color = sheep.getColor();
        //glColor4f(brightness * Sheep::COLOR[color][0], brightness * Sheep::COLOR[color][1], brightness * Sheep::COLOR[color][2], 1.0f);
        currentShaderColor = Sheep::COLOR[color];
        currentShaderDarkColor = Color(brightness, brightness, brightness);
        return 1;
    }
    else
    {
        return 0;
    }
}

