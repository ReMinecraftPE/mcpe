#pragma once

#include "EntityRenderer.hpp"

class ItemSpriteRenderer : public EntityRenderer
{
public:
	ItemSpriteRenderer(int sprite);
	void render(Entity*, float x, float y, float z, float a, float b) override;

public:
	int m_sprite;
};

