/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include <stddef.h>
#include "compat/LegacyCPP.hpp"
#include "world/level/TilePos.hpp"

struct Node
{
	int heapIdx;
	float g;
	float h;
	float f;
	Node* pCameFrom;
	TilePos tilePos;
	bool bClosed;
	int hash;

	Node()
	{
		init();
	}

	void setPos(const TilePos& tilePos)
	{
		this->tilePos = tilePos;
	}

	void setHash(int index)
	{
		hash = index;
	}

	void init()
	{
		heapIdx = -1;
		g = 0.0f;
		h = 0.0f;
		f = 0.0f;
		pCameFrom = nullptr;
		tilePos = TilePos(0, 0, 0);
		bClosed = false;
		hash = 0;
	}

	bool equals(Node* node)
	{
		return hash == node->hash && tilePos == node->tilePos;
	}

	float distanceTo(Node* node);
};
