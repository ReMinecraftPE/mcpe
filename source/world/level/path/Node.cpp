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
	float dx = float(node->tilePos.x) - float(tilePos.x);
	float dy = float(node->tilePos.y) - float(tilePos.y);
	float dz = float(node->tilePos.z) - float(tilePos.z);
	return Mth::sqrt(dx * dx + dy * dy + dz * dz);
}
