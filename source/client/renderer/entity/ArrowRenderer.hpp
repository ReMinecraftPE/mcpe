#pragma once

#include "EntityRenderer.hpp"

class ArrowRenderer : public EntityRenderer
{
public:
	ArrowRenderer();
	~ArrowRenderer();

	void render(Entity* ent, float x, float y, float z, float rot, float a) override;
};
