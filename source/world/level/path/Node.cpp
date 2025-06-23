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
	float dx = float(node->m_tilePos.x) - float(m_tilePos.x);
	float dy = float(node->m_tilePos.y) - float(m_tilePos.y);
	float dz = float(node->m_tilePos.z) - float(m_tilePos.z);
	return Mth::sqrt(dx * dx + dy * dy + dz * dz);
}
