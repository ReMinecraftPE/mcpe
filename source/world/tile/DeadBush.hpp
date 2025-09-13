#pragma once

#include "Bush.hpp"

class DeadBush : public Bush
{
public:
	DeadBush(int id, int texture);
	int getResource(int, Random*) const;
	bool mayPlace(const Level*, const TilePos& pos) const;
};
