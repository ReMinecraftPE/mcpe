#pragma once

#include "EntityRenderer.hpp"
#include "Tile.hpp"
#include "TileRenderer.hpp"
#include "TripodCamera.hpp"

class TripodCameraRenderer : public EntityRenderer
{
public:
	TripodCameraRenderer();

	void render(Entity*, float, float, float, float, float) override;
	
	static float getFlashTime(TripodCamera*, float f);

public:
	TileRenderer m_renderer;
	Tile m_tile;
	Cube m_cube;
};

