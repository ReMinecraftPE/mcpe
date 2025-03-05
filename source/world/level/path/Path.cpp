/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "Path.hpp"
#include "world/phys/Vec3.hpp"
#include "world/entity/Entity.hpp"

Path::Path()
{
	m_size = 0;
	m_pNodes = nullptr;
	m_index = 0;
}

Path::~Path()
{
	clear();
}

void Path::setNodes(Node** pNodes, int nodeCount)
{
	clear();
	m_size = nodeCount;
	m_pNodes = pNodes;

	for (int i = 0; i < nodeCount; i++)
	{
		// This won't cause a memory leak since the pNodes before are managed
		// by 
		// ...by what?
		Node* oldNode = pNodes[i];
		m_pNodes[i] = new Node;
		*m_pNodes[i] = *oldNode;
	}
}

Vec3 Path::getPos(const Entity& e, int index)
{
	Vec3 result(m_pNodes[index]->m_tilePos);
    float offset = float(int(e.m_bbWidth + 1.0f)) * 0.5f; // * 0.5f doesn't exist in 0.12.1, but does exist in later versions
	result.x += offset;
    result.z += offset;
    /*if (m_pNodes[index]->m_type == 6)
        result.y += 0.5f;*/

	return result;
}

Vec3 Path::currentPos(const Entity& e)
{
	return getPos(e, m_index);
}

Vec3 Path::currentPos()
{
	return Vec3(getCurrentNode()->m_tilePos);
}