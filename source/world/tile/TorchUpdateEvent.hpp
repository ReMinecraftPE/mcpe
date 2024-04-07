/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include <list>

struct TorchUpdateEvent
{
	int x, y, z;
	int time;
	
	TorchUpdateEvent()
	{
		x = y = z = time = 0;
	}
	
	TorchUpdateEvent(int _x, int _y, int _z, int _time)
	{
		x = _x;
		y = _y;
		z = _z;
		time = _time;
	}
};

typedef std::list<TorchUpdateEvent> TorchUpdateEvents;
