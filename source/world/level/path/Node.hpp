/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

struct Node
{
	int field_0;
	float field_4;
	float field_8;
	float field_C;
	Node* field_10;
	short m_x;
	short m_y;
	short m_z;
	bool field_1A;
	int m_hash;

	Node()
	{
		init();
	}

	void setPos(int x, int y, int z)
	{
		m_x = short(x);
		m_y = short(y);
		m_z = short(z);
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
		field_10 = nullptr;
		m_x = 0;
		m_y = 0;
		m_z = 0;
		field_1A = false;
		m_hash = 0;
	}

	bool equals(Node* node)
	{
		return m_hash == node->m_hash && m_x == node->m_x && m_y == node->m_y && m_z == node->m_z;
	}

	float distanceTo(Node* node);
};
