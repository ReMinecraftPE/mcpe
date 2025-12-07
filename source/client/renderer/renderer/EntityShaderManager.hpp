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

public:
	virtual Color getOverlayColor(const Entity& entity, float a);

protected:
	Materials m_shaderMaterials;
};
