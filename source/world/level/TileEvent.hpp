#pragma once

#include <stdint.h>
#include "TilePos.hpp"

struct TileEvent
{
	typedef int32_t Data;

	TilePos pos;

	Data b0;
	Data b1;

	TileEvent(const TilePos& pos, Data b0, Data b1)
		: pos(pos)
		, b0(b0)
		, b1(b1)
	{}
};
