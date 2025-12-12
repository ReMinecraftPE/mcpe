#pragma once

#include "common/math/Color.hpp"
#include "renderer/MaterialPtr.hpp"
#include "world/entity/Entity.hpp"

class EntityShaderManager
{
protected:
	class Materials
	{
	public:
		mce::MaterialPtr entity;
		mce::MaterialPtr entity_color_overlay;
		mce::MaterialPtr entity_alphatest;
		mce::MaterialPtr entity_static;

		Materials();
	};

protected:
	void _setupShaderParameters(float br, const Color& overlayColor);
	void _setupShaderParameters(const Entity& entity,
		const Color& overlayColor, const Color& tintColor,
		const Vec2& uvOffset, const Vec2& uvRot,
		const Vec2& glintUVScale, const Vec2& uvAnim, float a);
	void _setupShaderParameters(const Entity& entity, const Color& overlayColor, float a);
	void _setupShaderParameters(const Entity& entity, float a);
	void _setupShaderParameters(const Entity& entity, const Color& overlayColor, const Color& tintColor, float a);
	void _setupShaderParameters(const Entity& entity,
		const Color& overlayColor, const Color& tintColor,
		const Vec2& glintUVScale, float a);
	void _setupShaderParameters(const Entity& entity,
		const Color& tintColor,
		const Vec2& uvOffset, const Vec2& uvRot,
		const Vec2& glintUVScale, float a);
	void _setupShaderParameters(const Color& overlayColor, const Color& tintColor,
		const Vec2& uvOffset, const Vec2& uvRot,
		const Vec2& glintUVScale, const Vec2& uvAnim,
		float a);
	// For TileEntities
	void _setupShaderParameters(const LevelSource& source,
		const TilePos& pos,
		float a,
		const Vec2& uvScale);

public:
	virtual Color getOverlayColor(const Entity& entity, float a);

protected:
	Materials m_shaderMaterials;
};
