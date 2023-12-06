/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "Node.hpp"
#include "common/Mth.hpp"

float Node::distanceTo(Node* node)
{
	float dx = node->m_x - m_x;
	float dy = node->m_y - m_y;
	float dz = node->m_z - m_z;
	return Mth::sqrt(dx * dx + dy * dy + dz * dz);
}
