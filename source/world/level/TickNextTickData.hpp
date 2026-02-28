/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <stdint.h>
#include "common/Utils.hpp"
#include "world/level/TilePos.hpp"

struct TickNextTickData
{
	static int C;

	TickNextTickData(const TilePos& tilePos, TileID tileId);
	int hashCode() const;
	bool operator<(const TickNextTickData& other) const;
	bool operator==(const TickNextTickData& other) const;
	void setDelay(int32_t delay) { this->delay = delay; }

	int id;
	TilePos tilePos;
	TileID tileId;
	int32_t delay;
};

