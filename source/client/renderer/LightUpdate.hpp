#pragma once
#include "client/renderer/LightLayer.hpp"
#include "world/level/TilePos.hpp"

class TileSource;

class LightUpdate
{
private:
	const LightLayer& m_lightLayer;
	TileSource* m_source;
	TilePos m_min;
	TilePos m_max;

public:
	LightUpdate(TileSource&, const LightLayer&, const TilePos&, const TilePos&);

public:
	void update();
	void updateFast();

	bool expandIfCloseEnough(const TilePos& lowerPos, const TilePos& upperPos);
	void expandToContain(const TilePos& pos);
	void expandToContain(const TilePos& pos1, const TilePos& pos2);
};

