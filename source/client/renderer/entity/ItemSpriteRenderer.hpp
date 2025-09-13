#pragma once

#include "EntityRenderer.hpp"

class ItemSpriteRenderer : public EntityRenderer
{
public:
	ItemSpriteRenderer(int sprite);
	void render(Entity* entity, const Vec3& pos, float rot, float a) override;

public:
	int m_sprite;
};

