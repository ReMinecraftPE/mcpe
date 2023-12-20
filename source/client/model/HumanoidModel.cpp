/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "HumanoidModel.hpp"
#include "client/app/Minecraft.hpp"

HumanoidModel::HumanoidModel(float a, float b):
	Model(64, 32),
	m_head(0,  0),
	m_body(16, 16),
	m_arm1(40, 16),
	m_arm2(40, 16),
	m_leg1(0,  16),
	m_leg2(0,  16)
{
	field_234 = 0;

	m_head.setModel(this);
	m_body.setModel(this);
	m_arm1.setModel(this);
	m_arm2.setModel(this);
	m_leg1.setModel(this);
	m_leg2.setModel(this);

	m_head.addBox(-4, -8, -4, 8, 8, 8, a);
	m_head.setPos(0, b, 0);
	m_body.addBox(-4, 0, -2, 8, 12, 4);
	m_body.setPos(0, b, 0);
	m_arm1.addBox(-3, -2, -2, 4, 12, 4, a);
	m_arm1.setPos(-5, b + 2, 0);
	m_arm2.field_18 = true;
	m_arm2.addBox(-1, -2, -2, 4, 12, 4, a);
	m_arm2.setPos(5, b + 2, 0);
	m_leg1.addBox(-2, 0, -2, 4, 12, 4, a);
	m_leg1.setPos(-2, b + 12, 0);
	m_leg2.field_18 = true;
	m_leg2.addBox(-2, 0, -2, 4, 12, 4, a);
	m_leg2.setPos(2, b + 12, 0);
}

void HumanoidModel::onGraphicsReset()
{
	m_head.m_bCompiled = false;
	m_body.m_bCompiled = false;
	m_arm1.m_bCompiled = false;
	m_arm2.m_bCompiled = false;
	m_leg1.m_bCompiled = false;
	m_leg2.m_bCompiled = false;
}

void HumanoidModel::render(float a, float b, float c, float d, float e, float f)
{
	setupAnim(a, b, c, d, e, f);
	m_head.render(f);
	m_body.render(f);
	m_arm1.render(f);
	m_arm2.render(f);
	m_leg1.render(f);
	m_leg2.render(f);
}

void HumanoidModel::setupAnim(float a2, float a3, float a4, float a5, float a6, float a7)
{
	//a6 = pitch, a5 = yaw-ish
	m_head.m_rotY = a5 * 0.017453f;
	m_head.m_rotX = a6 * 0.017453f;
	if (m_head.m_rotX < -1.0f)
		m_head.m_rotX = -1.0f;
	if (m_head.m_rotX > 1.0f)
		m_head.m_rotX = 1.0f;
	float v12 = (a2 * 0.6662f) + 3.1416f;
	m_arm1.m_rotX = (Mth::cos(v12) * 2.0f * a3) * 0.5f;
	m_arm2.m_rotX = Mth::cos(a2 * 0.6662f) * 2.0f * a3 * 0.5f; // @HUH: multiplying by 2 and also by 1/2
	m_arm1.m_rotZ = 0.0f;
	m_arm2.m_rotZ = 0.0f;
	m_leg1.m_rotX = (Mth::cos(a2 * 0.6662f) * 1.4f) * a3;
	m_leg2.m_rotX = (Mth::cos(v12) * 1.4f) * a3;
	m_leg1.m_rotY = 0.0f;
	m_leg2.m_rotY = 0.0f;
	
	if (field_8)
	{
		float v15 = (3.1416f * -0.5f) * 0.4f;
		m_arm1.m_rotX += v15;
		m_arm2.m_rotX += v15;
		float v16 = (3.1416f * -0.5f) * 0.8f;
		m_leg1.m_rotX = v16;
		m_leg2.m_rotX = v16;
		m_leg1.m_rotY = (3.1416f * 0.5f) * 0.2f;
		m_leg2.m_rotY = (3.1416f * -0.5f) * 0.2f;
	}

	if (field_234)
		m_arm2.m_rotX = ((3.1416f * 0.5f) * -0.2f) + (m_arm2.m_rotX * 0.5f);
	if (field_235)
		m_arm1.m_rotX = ((3.1416f * 0.5f) * -0.2f) + (m_arm1.m_rotX * 0.5f);
	
	m_arm1.m_rotY = 0.0f;
	m_arm2.m_rotY = 0.0f;

	if (field_4 > -9990.0f)
	{
		m_body.m_rotY = Mth::sin(Mth::sqrt(field_4) * 3.1416f * 2.0f) * 0.2f;
		m_arm1.m_posZ = 5.0f * Mth::sin(m_body.m_rotY);
		m_arm1.m_posX = -5.0f * Mth::cos(m_body.m_rotY);
		m_arm2.m_posZ = -5.0f * Mth::sin(m_body.m_rotY);
		m_arm2.m_posX = 5.0f * Mth::cos(m_body.m_rotY);
		m_arm1.m_rotY = m_arm1.m_rotY + m_body.m_rotY;
		m_arm2.m_rotY = m_arm2.m_rotY + m_body.m_rotY;
		float o = 1.0f - field_4;
		m_arm2.m_rotX += m_body.m_rotY;
		m_arm1.m_rotX -= -((m_head.m_rotX - 0.7f) * Mth::sin(3.1416f * field_4)) * 0.75f + Mth::sin((1.0f - o * o * o * o) * 3.1416f) * 1.2f;
		m_arm1.m_rotY += m_body.m_rotY * 2.0f;
		m_arm1.m_rotZ = Mth::sin(field_4 * 3.1416f) * -0.4f;
	}

	if (m_bSneaking)
	{
		m_body.m_rotX = 0.5f;
		m_arm1.m_rotX += 0.4f;
		m_arm2.m_rotX += 0.4f;
		m_leg1.m_posZ = 4.0f;
		m_leg2.m_posZ = 4.0f;
		m_leg1.m_posY = 9.0f;
		m_leg2.m_posY = 9.0f;
		m_head.m_posY = 1.0f;
	}
	else
	{
		m_body.m_rotX = 0.0f;
		m_leg1.m_posZ = 0.0f;
		m_head.m_posY = 0.0f;
		m_leg2.m_posZ = 0.0f;
		m_leg1.m_posY = 12.0f;
		m_leg2.m_posY = 12.0f;
	}

	m_arm1.m_rotZ += Mth::cos(a4 * 0.09f) * 0.05f + 0.05f;
	m_arm2.m_rotZ -= Mth::cos(a4 * 0.09f) * 0.05f + 0.05f;
	m_arm1.m_rotX += Mth::sin(a4 * 0.067f) * 0.05f;
	m_arm2.m_rotX += Mth::sin(a4 * 0.067f) * -0.05f;
}

void HumanoidModel::setBrightness(float f)
{
	m_arm1.setBrightness(f);
	m_arm2.setBrightness(f);
	m_leg1.setBrightness(f);
	m_leg2.setBrightness(f);
	m_body.setBrightness(f);
	m_head.setBrightness(f);
}
