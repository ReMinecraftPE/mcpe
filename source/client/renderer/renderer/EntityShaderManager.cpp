#include "EntityShaderManager.hpp"

#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "world/entity/Mob.hpp"

EntityShaderManager::Materials::Materials()
{
	MATERIAL_PTR(switchable, entity);
	MATERIAL_PTR(switchable, entity_color_overlay);
	MATERIAL_PTR(switchable, entity_alphatest);
	MATERIAL_PTR(switchable, entity_static);
}

Color EntityShaderManager::getOverlayColor(const Entity& entity, float a)
{
    /*if (entity.isOnFire())
    {
        // Java does not apply an overlay color to entities that are on fire
        float v6 = powf((float)entity.m_fireTicks / 200, 0.3f);
        return Color(0.8f, 0.256f, 0.0f, (v6 + 0.35f) * 0.7f);
    }*/

    if (entity.isMob())
    {
        const Mob& mob = (const Mob&)entity;
        if (mob.m_hurtTime > 0 || mob.m_deathTime > 0)
        {
            // fucked up PE values (20% too red)
            //return Color(1.0f, 0.0f, 0.0f, 0.6f);

            // proper Java values from MobRenderer::render()
            float fBright = entity.getBrightness(a);
            return Color(fBright, 0.0f, 0.0f, 0.4f);
        }
    }

    return Color::NIL;
}