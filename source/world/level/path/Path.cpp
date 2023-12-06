/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "Path.hpp"

Path::Path()
{
	m_numNodes = 0;
	m_pNodes = nullptr;
	field_8 = 0;
}

Path::~Path()
{
	clear();
}

void Path::setNodes(Node** pNodes, int nodeCount)
{
	clear();
	m_numNodes = nodeCount;
	m_pNodes = pNodes;

	for (int i = 0; i < nodeCount; i++)
	{
		// TODO: We are using the pNodes array for storage but duplicating the pNodes?
		// This might cause a memory leak?
		Node* oldNode = pNodes[i];
		m_pNodes[i] = new Node;
		*m_pNodes[i] = *oldNode;
	}
}
