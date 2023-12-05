/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "PigModel.hpp"

PigModel::PigModel(float f) :
	QuadrupedModel(6, f)
{
	m_head.texOffs(16, 16);
	m_head.addBox(-2, 0, -9, 4, 3, 1, f);
	field_28C = 4.0f;
}

PigModel::~PigModel()
{
}
