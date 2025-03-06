/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "Node.hpp"

class Entity;

class Path
{
public:
	Path();
	~Path();

	void next() { m_index++; }
	bool isDone() const { return m_index >= m_size; }
	Node* get(int index) { return m_pNodes[index]; }
	int getSize() const { return m_size; }
	void setSize(int size) { m_size = size; }
	int getIndex() const { return m_index; }
	void setIndex(int index) { m_index = index; }
	void setNodes(Node** pNodes, int nodeCount);
	Vec3 getPos(const Entity& e, int index);
	Vec3 currentPos(const Entity& e);
	Vec3 currentPos();

	void clear()
	{
		if (m_pNodes)
		{
			for (int i = 0; i < m_size; i++)
				delete m_pNodes[i];

			delete[] m_pNodes;
		}
		
		m_pNodes = nullptr;
		m_size = 0;
		m_index = 0;
	}

	bool empty() { return m_size == 0 || m_pNodes == nullptr; }
	Node* getCurrentNode() { return get(m_index); }
	void rewind() { m_index = 0; }

private:
	int m_size;
	Node** m_pNodes;
	int m_index;
};
