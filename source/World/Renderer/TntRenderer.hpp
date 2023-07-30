#pragma once

#include "EntityRenderer.hpp"
#include "TileRenderer.hpp"

class TntRenderer : public EntityRenderer
{
public:
	TntRenderer();

	void render(Entity*, float, float, float, float, float) override;

public:
	TileRenderer m_tileRenderer;
};

