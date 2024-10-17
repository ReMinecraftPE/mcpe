/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include <limits>
#include "BinaryHeap.hpp"
#include "common/Mth.hpp"

void BinaryHeap::insert(Node* pNode)
{
	if (m_count == m_capacity)
	{
		m_capacity *= 2;
		Node** newItems = new Node * [m_capacity];

		for (int i = 0; i < m_count; i++)
		{
			newItems[i] = m_items[i];
		}

		SAFE_DELETE_ARRAY(m_items);

		m_items = newItems;
	}

	m_items[m_count] = pNode;
	pNode->field_0 = m_count;
	m_count++;

	inlined0(m_count - 1);
}

void BinaryHeap::inlined0(int num)
{
	Node* var2 = m_items[num];

	int var4;
	for (float var3 = var2->field_C; num > 0; num = var4) {
		var4 = (num - 1) >> 1;
		Node* var5 = m_items[var4];
		if (var3 >= var5->field_C) {
			break;
		}

		m_items[num] = var5;
		var5->field_0 = num;
	}

	m_items[num] = var2;
	var2->field_0 = num;
}

void BinaryHeap::downHeap(int num)
{
	Node* var2 = m_items[num];
	float var3 = var2->field_C;

	while (true) {
		int var4 = 1 + (num << 1);
		int var5 = var4 + 1;
		if (var4 >= m_count) {
			break;
		}

		Node* var6 = m_items[var4];
		float var7 = var6->field_C;
		Node* var8;
		float var9;
		if (var5 >= m_count) {
			var8 = nullptr;
			var9 = std::numeric_limits<float>::infinity();
		}
		else {
			var8 = m_items[var5];
			var9 = var8->field_C;
		}

		if (var7 < var9) {
			if (var7 >= var3) {
				break;
			}

			m_items[num] = var6;
			var6->field_0 = num;
			num = var4;
		}
		else {
			if (var9 >= var3) {
				break;
			}

			m_items[num] = var8;
			var8->field_0 = num;
			num = var5;
		}
	}

	m_items[num] = var2;
	var2->field_0 = num;
}
