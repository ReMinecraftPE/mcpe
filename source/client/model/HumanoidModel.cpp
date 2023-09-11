/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "HumanoidModel.hpp"
#include "client/app/Minecraft.hpp"

HumanoidModel::HumanoidModel(float a, float b):
	m_head(0,  0),
	m_body(16, 16),
	m_armL(40, 16),
	m_armR(40, 16),
	m_legL(0,  16),
	m_legR(0,  16)
{
	field_10BC = false;
	field_10BD = false;
	field_10BE = false;

	m_head.addBox(-4, -8, -4, 8, 8, 8, a);
	m_head.setPos(0, b, 0);
	m_body.addBox(-4, 0, -2, 8, 12, 4);
	m_body.setPos(0, b, 0);
	m_armL.addBox(-3, -2, -2, 4, 12, 4, a);
	m_armL.setPos(-5, b + 2, 0);
	m_armR.field_18 = true;
	m_armR.addBox(-1, -2, -2, 4, 12, 4, a);
	m_armR.setPos(5, b + 2, 0);
	m_legL.addBox(-2, 0, -2, 4, 12, 4, a);
	m_legL.setPos(-2, b + 12, 0);
	m_legR.field_18 = true;
	m_legR.addBox(-2, 0, -2, 4, 12, 4, a);
	m_legR.setPos(2, b + 12, 0);
}

void HumanoidModel::_logGraphics()
{
	Matrix m;

	if (Minecraft::customDebugId == 1)
	{
		// @NOTE: I think most of this function was ifdef'd/commented out
		m.fetchGL(GL_MODELVIEW_MATRIX);
	}
}

void HumanoidModel::onGraphicsReset()
{
	m_head.m_bCompiled = false;
	m_body.m_bCompiled = false;
	m_armL.m_bCompiled = false;
	m_armR.m_bCompiled = false;
	m_legL.m_bCompiled = false;
	m_legR.m_bCompiled = false;
}

void HumanoidModel::render(float a, float b, float c, float d, float e, float f)
{
	setupAnim(a, b, c, d, e, f);
	m_head.render(f);
	m_body.render(f);
	m_armL.render(f);
	m_armR.render(f);
	m_legL.render(f);
	m_legR.render(f);
	_logGraphics();
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
	m_armL.m_rotX = (Mth::cos(v12) * 2.0f * a3) * 0.5f;
	m_armR.m_rotX = Mth::cos(a2 * 0.6662f) * 2.0f * a3 * 0.5f; // @HUH: multiplying by 2 and also by 1/2
	m_armL.m_rotZ = 0.0f;
	m_armR.m_rotZ = 0.0f;
	m_legL.m_rotX = (Mth::cos(a2 * 0.6662f) * 1.4f) * a3;
	m_legR.m_rotX = (Mth::cos(v12) * 1.4f) * a3;
	m_legL.m_rotY = 0.0f;
	m_legR.m_rotY = 0.0f;
	
	if (field_8)
	{
		float v15 = (3.1416f * -0.5f) * 0.4f;
		m_armL.m_rotX += v15;
		m_armR.m_rotX += v15;
		float v16 = (3.1416f * -0.5f) * 0.8f;
		m_legL.m_rotX = v16;
		m_legR.m_rotX = v16;
		m_legL.m_rotY = (3.1416f * 0.5f) * 0.2f;
		m_legR.m_rotY = (3.1416f * -0.5f) * 0.2f;
	}

	if (field_10BC)
		m_armR.m_rotX = ((3.1416f * 0.5f) * -0.2f) + (m_armR.m_rotX * 0.5f);
	if (field_10BD)
		m_armL.m_rotX = ((3.1416f * 0.5f) * -0.2f) + (m_armL.m_rotX * 0.5f);
	
	m_armL.m_rotY = 0.0f;
	m_armR.m_rotY = 0.0f;

	if (field_4 > -9990.0f)
	{
		m_body.m_rotY = Mth::sin(Mth::sqrt(field_4) * 3.1416f * 2.0f) * 0.2f;
		m_armL.m_posZ = 5.0f * Mth::sin(m_body.m_rotY);
		m_armL.m_posX = -5.0f * Mth::cos(m_body.m_rotY);
		m_armR.m_posZ = -5.0f * Mth::sin(m_body.m_rotY);
		m_armR.m_posX = 5.0f * Mth::cos(m_body.m_rotY);
		m_armL.m_rotY = m_armL.m_rotY + m_body.m_rotY;
		m_armR.m_rotY = m_armR.m_rotY + m_body.m_rotY;
		float o = 1.0f - field_4;
		m_armR.m_rotX += m_body.m_rotY;
		m_armL.m_rotX -= -((m_head.m_rotX - 0.7f) * Mth::sin(3.1416f * field_4)) * 0.75f + Mth::sin((1.0f - o * o * o * o) * 3.1416f) * 1.2f;
		m_armL.m_rotY += m_body.m_rotY * 2.0f;
		m_armL.m_rotZ = Mth::sin(field_4 * 3.1416f) * -0.4f;
	}

	if (field_10BE) // sneaking?
	{
		m_body.m_rotX = 0.5f;
		m_armL.m_rotX += 0.4f;
		m_armR.m_rotX += 0.4f;
		m_legL.m_posZ = 4.0f;
		m_legR.m_posZ = 4.0f;
		m_legL.m_posY = 9.0f;
		m_legR.m_posY = 9.0f;
		m_head.m_posY = 1.0f;
	}
	else
	{
		m_body.m_rotX = 0.0f;
		m_legL.m_posZ = 0.0f;
		m_head.m_posY = 0.0f;
		m_legR.m_posZ = 0.0f;
		m_legL.m_posY = 12.0f;
		m_legR.m_posY = 12.0f;
	}

	m_armL.m_rotZ += Mth::cos(a4 * 0.09f) * 0.05f + 0.05f;
	m_armR.m_rotZ -= Mth::cos(a4 * 0.09f) * 0.05f + 0.05f;
	m_armL.m_rotX += Mth::sin(a4 * 0.067f) * 0.05f;
	m_armR.m_rotX += Mth::sin(a4 * 0.067f) * -0.05f;
}

void HumanoidModel::setBrightness(float f)
{
	m_armL.setBrightness(f);
	m_armR.setBrightness(f);
	m_legL.setBrightness(f);
	m_legR.setBrightness(f);
	m_body.setBrightness(f);
	m_head.setBrightness(f);
}
