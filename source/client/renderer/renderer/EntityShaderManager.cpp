#include "EntityShaderManager.hpp"

#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "renderer/GlobalConstantBuffers.hpp"
#include "world/entity/Mob.hpp"
#include "world/level/Level.hpp"

EntityShaderManager::Materials::Materials()
{
	MATERIAL_PTR(switchable, entity);
	MATERIAL_PTR(switchable, entity_color_overlay);
	MATERIAL_PTR(switchable, entity_alphatest);
	MATERIAL_PTR(switchable, entity_static);
}

void EntityShaderManager::_setupShaderParameters(float br, const Color& overlayColor)
{
#ifdef FEATURE_GFX_SHADERS
	mce::GlobalConstantBuffers& globalBuffers = mce::GlobalConstantBuffers::getInstance();
	mce::EntityConstants& cEntity = globalBuffers.m_entityConstants;

	Color tileLightColor = Color(br, br, br, 1.0f);
	cEntity.TILE_LIGHT_COLOR->setData(&tileLightColor);

	cEntity.OVERLAY_COLOR->setData(&overlayColor);

	cEntity.sync();
#endif
}

void EntityShaderManager::_setupShaderParameters(const Entity& entity,
	const Color& overlayColor, const Color& tintColor,
	const Vec2& uvOffset, const Vec2& uvRot,
	const Vec2& glintUVScale, const Vec2& uvAnim, float a)
{
#ifdef FEATURE_GFX_SHADERS
	mce::GlobalConstantBuffers& globalBuffers = mce::GlobalConstantBuffers::getInstance();
	mce::EntityConstants& cEntity = globalBuffers.m_entityConstants;

	Color tileLightColor = Color::WHITE * entity.getBrightness(a);

	if (a != 1.0f)
	{
		tileLightColor *= a;
	}

	cEntity.TILE_LIGHT_COLOR->setData(&tileLightColor);
	cEntity.OVERLAY_COLOR->setData(&overlayColor);
	cEntity.CHANGE_COLOR->setData(&tintColor);
	cEntity.UV_OFFSET->setData(&uvOffset);
	cEntity.UV_ROTATION->setData(&uvRot);
	Vec2 glintUV = glintUVScale * 0.017453f;
	cEntity.GLINT_UV_SCALE->setData(&glintUV);
	cEntity.UV_ANIM->setData(&uvAnim);

	cEntity.sync();
#endif
}

void EntityShaderManager::_setupShaderParameters(const Entity& entity, const Color& overlayColor, float a)
{
	_setupShaderParameters(entity, overlayColor, entity.m_tintColor, a);
}

void EntityShaderManager::_setupShaderParameters(const Entity& entity, float a)
{
	_setupShaderParameters(entity, getOverlayColor(entity, a), a);
}

void EntityShaderManager::_setupShaderParameters(const Entity& entity, const Color& overlayColor, const Color& tintColor, float a)
{
	_setupShaderParameters(entity,
		overlayColor, tintColor,
		Vec2::ZERO, Vec2::ZERO,
		Vec2::ONE, Vec2::ZERO,
		1.0f);
}

void EntityShaderManager::_setupShaderParameters(const Entity& entity,
	const Color& overlayColor, const Color& tintColor,
	const Vec2& glintUVScale, float a)
{
	_setupShaderParameters( entity,
		overlayColor, tintColor,
		Vec2::ZERO, Vec2::ZERO,
		Vec2::ONE, glintUVScale,
		a);
}

void EntityShaderManager::_setupShaderParameters(const Entity& entity,
	const Color& tintColor,
	const Vec2& uvOffset, const Vec2& uvRot,
	const Vec2& glintUVScale, float a)
{
	_setupShaderParameters(entity,
		getOverlayColor(entity, a), tintColor,
		uvOffset, uvRot,
		glintUVScale, Vec2::ZERO,
		1.0f);
}

void EntityShaderManager::_setupShaderParameters(const Color& overlayColor, const Color& tintColor,
	const Vec2& uvOffset, const Vec2& uvRot,
	const Vec2& glintUVScale, const Vec2& uvAnim,
	float a)
{
#ifdef FEATURE_GFX_SHADERS
	mce::GlobalConstantBuffers& globalBuffers = mce::GlobalConstantBuffers::getInstance();
	mce::EntityConstants& cEntity = globalBuffers.m_entityConstants;

	cEntity.TILE_LIGHT_COLOR->setData(&Color::WHITE);
	cEntity.OVERLAY_COLOR->setData(&overlayColor);
	cEntity.CHANGE_COLOR->setData(&tintColor);
	cEntity.UV_OFFSET->setData(&uvOffset);
	cEntity.UV_ROTATION->setData(&uvRot);
	Vec2 glintUV = glintUVScale * 0.017453f;
	cEntity.GLINT_UV_SCALE->setData(&glintUV);
	cEntity.UV_ANIM->setData(&uvAnim);

	cEntity.sync();
#endif
}

void EntityShaderManager::_setupShaderParameters(const LevelSource& source,
	const TilePos& pos,
	float a,
	const Vec2& uvScale)
{
#ifdef FEATURE_GFX_SHADERS
	mce::GlobalConstantBuffers& globalBuffers = mce::GlobalConstantBuffers::getInstance();
	mce::EntityConstants& cEntity = globalBuffers.m_entityConstants;

	cEntity.TILE_LIGHT_COLOR->setData(&Color::WHITE); // source.getLightColor(pos, 0)
	cEntity.OVERLAY_COLOR->setData(&Color::NIL);
	cEntity.CHANGE_COLOR->setData(&Color::WHITE);
	cEntity.GLINT_UV_SCALE->setData(&uvScale);

	cEntity.sync();
#endif
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
