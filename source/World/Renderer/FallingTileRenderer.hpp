#pragma once
#if defined(ENH_ALLOW_SAND_GRAVITY)

#include "EntityRenderer.hpp"
#include "TileRenderer.hpp"

class FallingTileRenderer : public EntityRenderer
{
public:
	FallingTileRenderer();

	void render(Entity*, float, float, float, float, float) override;

public:
	TileRenderer m_tileRenderer;
};

#endif
