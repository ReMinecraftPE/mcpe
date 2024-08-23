#pragma once

#include "Tile.hpp"

class WorkBenchTile : public Tile
{
public:
	WorkBenchTile(int id, Material*);
	int getTexture(int paramSide) override;
	int use(Level* pLevel, int x, int y, int z, Player* player) override;
};
