/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "CreeperModel.hpp"
#include "common/Mth.hpp"

CreeperModel::CreeperModel() :
	Model(64, 32),
	m_head(this, 0, 0),
	//m_hair(this, 32, 0),
	m_body(this, 16, 16),
	m_leg1(this, 0, 16),
	m_leg2(this, 0, 16),
	m_leg3(this, 0, 16),
	m_leg4(this, 0, 16)
{
	m_head.addBox(-4, -8, -4, 8, 8, 8);
	m_head.setPos(0, 4, 0);
	m_body.addBox(-4, 0, -2, 8, 12, 4);
	m_body.setPos(0, 4, 0);
	m_leg1.addBox(-2, 0, -2, 4, 6, 4);
	m_leg1.setPos(-2, 16, 4);
	m_leg2.addBox(-2, 0, -2, 4, 6, 4);
	m_leg2.setPos(2, 16, 4);
	m_leg3.addBox(-2, 0, -2, 4, 6, 4);
	m_leg3.setPos(-2, 16, -4);
	m_leg4.addBox(-2, 0, -2, 4, 6, 4);
	m_leg4.setPos(2, 16, -4);

	float g = 0.0f;
	// Creepers have just always been floating in Java.
	// Setting it to 6 fixes this, but makes the creeper look short.
	int yo = 4;
	m_head.addBox(-4, -8, -4, 8, 8, 8, g);
	m_head.setPos(0, yo, 0);
	//m_hair.addBox(-4, -8, -4, 8, 8, 8, g + 0.5f);
	//m_hair.setPos(0, yo, 0);
	m_body.addBox(-4, 0, -2, 8, 12, 4, g);
	m_body.setPos(0, yo, 0);
	m_leg1.addBox(-2, 0, -2, 4, 6, 4, g);
	m_leg1.setPos(-2, 12 + yo, 4);
	m_leg2.addBox(-2, 0, -2, 4, 6, 4, g);
	m_leg2.setPos(2, 12 + yo, 4);
	m_leg3.addBox(-2, 0, -2, 4, 6, 4, g);
	m_leg3.setPos(-2, 12 + yo, -4);
	m_leg4.addBox(-2, 0, -2, 4, 6, 4, g);
	m_leg4.setPos(2, 12 + yo, -4);
}

CreeperModel::~CreeperModel()
{
}

void CreeperModel::render(float a, float b, float c, float d, float e, float f)
{
	setupAnim(a, b, c, d, e, f);

	m_head.render(f);
	m_body.render(f);
	m_leg1.render(f);
	m_leg2.render(f);
	m_leg3.render(f);
	m_leg4.render(f);
}

void CreeperModel::setupAnim(float a, float b, float c, float d, float e, float f)
{
	m_head.m_rot.y = d / (180.0f / 3.1416f);
	m_head.m_rot.x = e / (180.0f / 3.1416f);

	float footAng = (Mth::cos(a * 0.6662f) * 1.4f) * b;
	m_leg1.m_rot.x = footAng;
	m_leg4.m_rot.x = footAng;
	m_leg2.m_rot.x = -footAng;
	m_leg3.m_rot.x = -footAng;
}