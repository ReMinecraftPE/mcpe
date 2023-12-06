/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "Node.hpp"

class Path
{
public:
	Path();
	~Path();

	void setNodes(Node** pNodes, int nodeCount);

	void clear()
	{
		if (m_pNodes)
		{
			for (int i = 0; i < m_numNodes; i++)
				delete m_pNodes[i];

			delete[] m_pNodes;
		}
		
		m_pNodes = nullptr;
		m_numNodes = 0;
		field_8 = 0;
	}

	bool empty()
	{
		return m_numNodes == 0 || m_pNodes == nullptr;
	}

	Node* getCurrentNode()
	{
		return m_pNodes[field_8];
	}

	void advance()
	{
		field_8++;
	}

	void rewind()
	{
		field_8 = 0;
	}

	bool endOfPath()
	{
		return field_8 >= m_numNodes;
	}

private:
	int m_numNodes;
	Node** m_pNodes;
	int field_8;
};
