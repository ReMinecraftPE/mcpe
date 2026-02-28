/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TickNextTickData.hpp"

int TickNextTickData::C;

TickNextTickData::TickNextTickData(const TilePos& tilePos, TileID tileId)
	: id(++C) //@NOTE: not C++
	, tilePos(tilePos)
	, tileId(tileId)
{
#ifndef ORIGINAL_CODE
	delay = 0;
#endif
}

int TickNextTickData::hashCode() const
{
	return tileId + ((tilePos.y + ((tilePos.z + (tilePos.x << 10)) << 7)) << 8);
}

bool TickNextTickData::operator<(const TickNextTickData& other) const
{
	if (delay < other.delay)
		return true;
	if (delay > other.delay)
		return false;

	return id < other.id;
}

bool TickNextTickData::operator==(const TickNextTickData& other) const
{
	return
		tilePos  == other.tilePos  &&
		tileId == other.tileId;
}
