#pragma once

#include "Bush.hpp"

class DeadBush : public Bush
{
public:
	DeadBush(int id, int texture);

public:
	int getResource(TileData, Random*) const override;
	bool mayPlace(TileSource*, const TilePos& pos) const override;
};
