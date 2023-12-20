/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "Node.hpp"

class BinaryHeap
{
public:
	BinaryHeap()
	{
		m_count = 0;
		m_capacity = 1024;
		m_items = new Node*[m_capacity];
	}

	~BinaryHeap()
	{
		if (m_items)
			delete[] m_items;
	}

	void insert(Node* pNode);
	void inlined0(int i);
	void downHeap(int i);

	Node* removeTop() {
		Node* pNode = m_items[0];
		m_items[0] = m_items[--m_count];
		m_items[m_count] = 0;

		if (m_count > 0)
			downHeap(0);

		pNode->field_0 = -1;
		return pNode;
	}

	void clear() {
		m_count = 0;
	}
	
	int size() const {
		return m_count;
	}

	void setDistance(Node* pNode, float distance) {
		float oldDistance = pNode->field_C;
		pNode->field_C = distance;

		if (oldDistance >= distance)
			downHeap(pNode->field_0);
		else
			inlined0(pNode->field_0);
	}

private:
	Node** m_items;
	int m_count;
	int m_capacity;
};
