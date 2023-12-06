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
	float dx = float(node->m_x) - float(m_x);
	float dy = float(node->m_y) - float(m_y);
	float dz = float(node->m_z) - float(m_z);
	return Mth::sqrt(dx * dx + dy * dy + dz * dz);
}
