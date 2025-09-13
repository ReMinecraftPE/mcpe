#pragma once

#include "EntityRenderer.hpp"

class ArrowRenderer : public EntityRenderer
{
public:
	ArrowRenderer();
	~ArrowRenderer();

	void render(Entity* ent, const Vec3& pos, float rot, float a) override;
};
