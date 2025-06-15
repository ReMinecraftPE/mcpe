/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "QuadrupedModel.hpp"
#include "common/Mth.hpp"

QuadrupedModel::QuadrupedModel(int i, float f) :
	Model(64, 32),
	m_head(this, 0, 0),
	m_hair(this, 0, 0),
	m_body(this, 28, 8),
	m_leg1(this, 0, 16),
	m_leg2(this, 0, 16),
	m_leg3(this, 0, 16),
	m_leg4(this, 0, 16)
{
	field_28C = 8.0f;
	field_290 = 4.0f;

	m_head.addBox(-4, -4, -8, 8, 8, 8, f);
	m_head.setPos(0, 18 - float(i), -6);
	m_body.addBox(-5, -10, -7, 10, 16, 8, f);
	m_body.setPos(0, 17 - float(i), 2);
	m_leg1.addBox(-2, 0, -2, 4, i, 4, f);
	m_leg1.setPos(-3, 24 - float(i), 7);
	m_leg2.addBox(-2, 0, -2, 4, i, 4, f);
	m_leg2.setPos(3, 24 - float(i), 7);
	m_leg3.addBox(-2, 0, -2, 4, i, 4, f);
	m_leg3.setPos(-3, 24 - float(i), -5);
	m_leg4.addBox(-2, 0, -2, 4, i, 4, f);
	m_leg4.setPos(3, 24 - float(i), -5);
}

QuadrupedModel::~QuadrupedModel()
{
}

// NOTE: Model::render now takes an Entity pointer... It's unused, though...
void QuadrupedModel::render(float a, float b, float c, float d, float e, float f)
{
	setupAnim(a, b, c, d, e, f);

	if (m_bIsBaby)
	{
		glPushMatrix();
		glTranslatef(0.0f, f * field_28C, f * field_290);
		m_head.render(f);
		glPopMatrix();
		glPushMatrix();
		glScalef(0.5f, 0.5f, 0.5f);
		glTranslatef(0.0f, f * 24.0f, 0.0f);
		m_body.render(f);
		m_leg1.render(f);
		m_leg2.render(f);
		m_leg3.render(f);
		m_leg4.render(f);
		glPopMatrix();
	}
	else
	{
		m_head.render(f);
		m_body.render(f);
		m_leg1.render(f);
		m_leg2.render(f);
		m_leg3.render(f);
		m_leg4.render(f);
	}
}

void QuadrupedModel::setupAnim(float a2, float a3, float a4, float a5, float a6, float a7)
{
	m_head.m_rot.x = a6 / (float)(180.0f / 3.1416f);
	m_head.m_rot.y = a5 / (float)(180.0f / 3.1416f);
	m_body.m_rot.x = 90.0f / (float)(180.0f / 3.1416f);

	float footAng = (Mth::cos(a2 * 0.6662f) * 1.4f) * a3;
	m_leg1.m_rot.x = footAng;
	m_leg4.m_rot.x = footAng;
	m_leg2.m_rot.x = -footAng;
	m_leg3.m_rot.x = -footAng;
}
