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
	int field_0;
	float field_4;
	float field_8;
	float field_C;
	Node* m_pCameFrom;
	TilePos m_tilePos;
	bool m_bClosed;
	int m_hash;

	Node()
	{
		init();
	}

	void setPos(const TilePos& tilePos)
	{
		m_tilePos = tilePos;
	}

	void setHash(int index)
	{
		m_hash = index;
	}

	void init()
	{
		field_0 = -1;
		field_4 = 0.0f;
		field_8 = 0.0f;
		field_C = 0.0f;
		m_pCameFrom = nullptr;
		m_tilePos = TilePos(0, 0, 0);
		m_bClosed = false;
		m_hash = 0;
	}

	bool equals(Node* node)
	{
		return m_hash == node->m_hash && m_tilePos == node->m_tilePos;
	}

	float distanceTo(Node* node);
};
