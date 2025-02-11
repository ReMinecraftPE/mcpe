#include "SheepRenderer.hpp"

SheepRenderer::SheepRenderer(Model* pModel, Model* pArmor, float f) : MobRenderer(pModel, f)
{
	setArmor(pArmor);
}

SheepRenderer::~SheepRenderer()
{
}

int SheepRenderer::prepareArmor(Mob* mob, int layer, float a)
{
    Sheep* pSheep = (Sheep*)mob;
    if (layer == 0 && !pSheep->isSheared())
    {
        bindTexture("/mob/sheep_fur.png");
        float brightness = pSheep->getBrightness(a);
        int color = pSheep->getColor();
        glColor3f(brightness * Sheep::COLOR[color][0], brightness * Sheep::COLOR[color][1], brightness * Sheep::COLOR[color][2]);
        return 1;
    }
    else
    {
        return 0;
    }
}

