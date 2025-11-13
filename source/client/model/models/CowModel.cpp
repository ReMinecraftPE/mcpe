/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "CowModel.hpp"

CowModel::CowModel() :
	QuadrupedModel(12, 0.0f)
{
    m_head.clear();
	m_head.texOffs(0, 0);

	// head
	m_head.addBox(-4, -4, -6, 8, 8, 6);
	m_head.setPos(0, 4, -8);

	// horns
	m_head.texOffs(22, 0);
	m_head.addBox(-5, -5, -4, 1, 3, 1);
	m_head.texOffs(22, 0);
	m_head.addBox(4, -5, -4, 1, 3, 1);

    m_body.clear();
	m_body.texOffs(18, 4);

	// torso
	m_body.addBox(-6, -10, -7, 12, 18, 10);
	m_body.setPos(0, 5, 2);

	// udders
	m_body.texOffs(52, 0);
	m_body.addBox(-2, 2, -8, 4, 6, 1);

	m_leg1.m_pos.x -= 1.0f;
	m_leg2.m_pos.x += 1.0f;
	m_leg3.m_pos.x -= 1.0f;
	m_leg4.m_pos.x += 1.0f;
	field_290 += 2.0f;
}

CowModel::~CowModel()
{
}
