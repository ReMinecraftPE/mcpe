/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "ChickenModel.hpp"
#include "common/Mth.hpp"

ChickenModel::ChickenModel() :
	Model(64, 32),
	m_head(this, 0, 0),
	//m_hair(this, 0, 0),
	m_body(this, 0, 9),
	m_leg1(this, 26, 0),
	m_leg2(this, 26, 0),
	m_wing1(this, 24, 13),
	m_wing2(this, 24, 13),
	m_beak(this, 14, 0),
	m_wattle(this, 14, 4) // Yes, it's called a wattle. Look it up.
{
	m_head.addBox(-2, -6, -2, 4, 6, 3);
	m_head.setPos(0, 15, -4);
	m_beak.addBox(-2, -4, -4, 4, 2, 2, 0);
	m_beak.setPos(0, 15, -4);
	m_wattle.addBox(-1, -2, -3, 2, 2, 2, 0);
	m_wattle.setPos(0, 15, -4);
	m_body.addBox(-3, -4, -3, 6, 8, 6, 0);
	m_body.setPos(0, 16, 0);
	m_leg1.addBox(-1, 0, -3, 3, 5, 3);
	m_leg1.setPos(-2, 19, 1);
	m_leg2.addBox(-1, 0, -3, 3, 5, 3);
	m_leg2.setPos(1, 19, 1);
	m_wing1.addBox(0, 0, -3, 1, 4, 6);
	m_wing1.setPos(-4, 13, 0);
	m_wing2.addBox(-1, 0, -3, 1, 4, 6);
	m_wing2.setPos(4, 13, 0);
}

ChickenModel::~ChickenModel()
{
}

void ChickenModel::render(float a, float b, float c, float d, float e, float f)
{
	setupAnim(a, b, c, d, e, f);

	if (m_bIsBaby)
	{
		glPushMatrix();
		glTranslatef(0.0f, f * 5.0f, f * 2.0f);
		m_head.render(f);
		m_beak.render(f);
		m_wattle.render(f);
		glPopMatrix();
		glPushMatrix();
		glScalef(0.5f, 0.5f, 0.5f);
		glTranslatef(0.0f, f * 24.0f, 0.0f);
		m_body.render(f);
		m_leg1.render(f);
		m_leg2.render(f);
		m_wing1.render(f);
		m_wing2.render(f);
		glPopMatrix();
	}
	else
	{
		m_head.render(f);
		m_beak.render(f);
		m_wattle.render(f);
		m_body.render(f);
		m_leg1.render(f);
		m_leg2.render(f);
		m_wing1.render(f);
		m_wing2.render(f);
	}
}

void ChickenModel::setupAnim(float a, float b, float c, float d, float e, float f)
{
	// TODO: Why do I need to invert the rotation again?
	float rotX = e / (-180.0f / 3.1416f);
	m_head.m_rot.x = -rotX;
	m_beak.m_rot.x = -rotX;
	m_wattle.m_rot.x = -rotX;

	float rotY = d / (180.0f / 3.1416f);
	m_head.m_rot.y = rotY;
	m_beak.m_rot.y = rotY;
	m_wattle.m_rot.y = rotY;

	m_body.m_rot.x = 90.0f / (180.0f / 3.1416f);
	m_wing1.m_rot.z = c;
	m_wing2.m_rot.z = -c;

	float footAng = (Mth::cos(a * 0.6662f) * 1.4f) * b;
	m_leg1.m_rot.x = footAng;
	m_leg2.m_rot.x = -footAng;
}