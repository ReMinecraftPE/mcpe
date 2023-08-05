/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "HumanoidModel.hpp"
#include "Minecraft.hpp"

HumanoidModel::HumanoidModel(float a, float b):
	m_head(0,  0),
	m_body(16, 16),
	m_armL(40, 16),
	m_armR(40, 16),
	m_legL(0,  16),
	m_legR(0,  16)
{
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

void HumanoidModel::renderHorrible(float a, float b, float c, float d, float e, float f)
{
	setupAnim(a, b, c, d, e, f);
	m_head.renderHorrible(f);
	m_body.renderHorrible(f);
	m_armL.renderHorrible(f);
	m_armR.renderHorrible(f);
	m_legL.renderHorrible(f);
	m_legR.renderHorrible(f);
	_logGraphics();
}

void HumanoidModel::setupAnim(float a2, float a3, float a4, float a5, float a6, float a7)
{
	//a6 = pitch, a5 = yaw-ish
	m_head.field_10 = a5 * 0.017453f;
	m_head.field_C = a6 * 0.017453f;
	if (m_head.field_C < -1.0f)
		m_head.field_C = -1.0f;
	if (m_head.field_C > 1.0f)
		m_head.field_C = 1.0f;
	float v12 = (a2 * 0.6662f) + 3.1416f;
	m_armL.field_C = (Mth::cos(v12) * 2.0f * a3) * 0.5f;
	m_armR.field_C = Mth::cos(a2 * 0.6662f) * 2.0f * a3 * 0.5f; // @HUH: multiplying by 2 and also by 1/2
	m_armL.field_14 = 0.0f;
	m_armR.field_14 = 0.0f;
	m_legL.field_C = (Mth::cos(a2 * 0.6662f) * 1.4f) * a3;
	m_legR.field_C = (Mth::cos(v12) * 1.4f) * a3;
	m_legL.field_10 = 0.0f;
	m_legR.field_10 = 0.0f;
	
	if (field_8)
	{
		float v15 = (3.1416f * -0.5f) * 0.4f;
		m_armL.field_C += v15;
		m_armR.field_C += v15;
		float v16 = (3.1416f * -0.5f) * 0.8f;
		m_legL.field_C = v16;
		m_legR.field_C = v16;
		m_legL.field_10 = (3.1416f * 0.5f) * 0.2f;
		m_legR.field_10 = (3.1416f * -0.5f) * 0.2f;
	}

	if (field_10BC)
		m_armR.field_C = ((3.1416f * 0.5f) * -0.2f) + (m_armR.field_C * 0.5f);
	if (field_10BD)
		m_armL.field_C = ((3.1416f * 0.5f) * -0.2f) + (m_armL.field_C * 0.5f);
	
	m_armL.field_10 = 0.0f;
	m_armR.field_10 = 0.0f;

	if (field_4 > -9990.0f)
	{
		m_body.field_10 = Mth::sin(Mth::sqrt(field_4) * 3.1416f * 2.0f) * 0.2f;
		m_armL.m_posZ = 5.0f * Mth::sin(m_body.field_10);
		m_armL.m_posX = -5.0f * Mth::cos(m_body.field_10);
		m_armR.m_posZ = -5.0f * Mth::sin(m_body.field_10);
		m_armR.m_posX = 5.0f * Mth::cos(m_body.field_10);
		m_armL.field_10 = m_armL.field_10 + m_body.field_10;
		m_armR.field_10 = m_armR.field_10 + m_body.field_10;
		float o = 1.0f - field_4;
		m_armR.field_C += m_body.field_10;
		m_armL.field_C -= -((m_head.field_C - 0.7f) * Mth::sin(3.1416f * field_4)) * 0.75f + Mth::sin((1.0f - o * o * o * o) * 3.1416f) * 1.2f;
		m_armL.field_10 += m_body.field_10 * 2.0f;
		m_armL.field_14 = Mth::sin(field_4 * 3.1416f) * -0.4f;
	}

	if (field_10BE) // sneaking?
	{
		m_body.field_C = 0.5f;
		m_armL.field_C += 0.4f;
		m_armR.field_C += 0.4f;
		m_legL.m_posZ = 4.0f;
		m_legR.m_posZ = 4.0f;
		m_legL.m_posY = 9.0f;
		m_legR.m_posY = 9.0f;
		m_head.m_posY = 1.0f;
	}
	else
	{
		m_body.field_C = 0.0f;
		m_legL.m_posZ = 0.0f;
		m_head.m_posY = 0.0f;
		m_legR.m_posZ = 0.0f;
		m_legL.m_posY = 12.0f;
		m_legR.m_posY = 12.0f;
	}

	m_armL.field_14 += Mth::cos(a4 * 0.09f) * 0.05f + 0.05f;
	m_armR.field_14 -= Mth::cos(a4 * 0.09f) * 0.05f + 0.05f;
	m_armL.field_C += Mth::sin(a4 * 0.067f) * 0.05f;
	m_armR.field_C += Mth::sin(a4 * 0.067f) * -0.05f;
}

/* Keeping the original around just in case:

void HumanoidModel::setupAnim(float a2, float a3, float a4, float a5, float a6, float a7)
{
	this->m_head.field_10 = a5 * 0.017453f;
	float v11 = a6 * 0.017453f;
	if ((a6 * 0.017453f) <= 1.0f)
	{
		if (v11 < -1.0f)
			v11 = -1.0f;
	}
	else
	{
		v11 = 1.0f;
	}
	this->m_head.field_C = v11;
	float v12 = (a2 * 0.6662f) + 3.1416f;
	float v13 = Mth::cos(v12);
	this->m_armL.field_C = ((v13 + v13) * a3) * 0.5f;
	float v14 = Mth::cos(a2 * 0.6662f);
	this->m_armR.field_C = ((v14 + v14) * a3) * 0.5f;
	this->m_armL.field_14 = 0.0f;
	this->m_armR.field_14 = 0.0f;
	this->m_legL.field_C = (Mth::cos(a2 * 0.6662f) * 1.4f) * a3;
	this->m_legR.field_C = (Mth::cos(v12) * 1.4f) * a3;
	this->m_legL.field_10 = 0.0f;
	this->m_legR.field_10 = 0.0f;
	if (this->field_8)
	{
		float v15 = (3.1416f * -0.5f) * 0.4f;
		this->m_armL.field_C = this->m_armL.field_C + v15;
		this->m_armR.field_C = this->m_armR.field_C + v15;
		float v16 = (3.1416f * -0.5f) * 0.8f;
		this->m_legL.field_C = v16;
		this->m_legR.field_C = v16;
		this->m_legL.field_10 = (3.1416f * 0.5f) * 0.2f;
		this->m_legR.field_10 = (3.1416f * -0.5f) * 0.2f;
	}
	if (this->field_10BC)
		this->m_armR.field_C = ((3.1416f * 0.5f) * -0.2f) + (this->m_armR.field_C * 0.5f);
	if (this->field_10BD)
		this->m_armL.field_C = ((3.1416f * 0.5f) * -0.2f) + (this->m_armL.field_C * 0.5f);

	this->m_armL.field_10 = 0.0f;
	this->m_armR.field_10 = 0.0f;
	if (field_4 > -9990.0f)
	{
		float v22 = Mth::sqrt(field_4);
		float v23 = Mth::sin((v22 * 3.1416f) + (v22 * 3.1416f));
		this->m_body.field_10 = v23 * 0.2f;
		float v24 = Mth::sin(v23 * 0.2f) * 5.0f;
		float bf10_1 = this->m_body.field_10;
		this->m_armL.m_posZ = v24;
		float v26 = Mth::cos(bf10_1) * -5.0f;
		float bf10_2 = this->m_body.field_10;
		this->m_armL.m_posX = v26;
		float v28 = Mth::sin(bf10_2) * -5.0f;
		float bf10_3 = this->m_body.field_10;
		this->m_armR.m_posZ = v28;
		this->m_armR.m_posX = Mth::cos(bf10_3) * 5.0f;
		float bf10_4 = this->m_body.field_10;
		this->m_armL.field_10 = this->m_armL.field_10 + bf10_4;
		this->m_armR.field_10 = this->m_armR.field_10 + bf10_4;
		float v31 = this->m_armR.field_C + bf10_4;
		float v32 = 1.0f - this->field_4;
		this->m_armR.field_C = v31;
		float v33 = Mth::sin((1.0f - ((v32 * v32) * (v32 * v32))) * 3.1416f);
		float v34 = Mth::sin(3.1416f * this->field_4);
		float bf10_times_2 = this->m_body.field_10 + this->m_body.field_10;
		this->m_armL.field_C = this->m_armL.field_C - ((-((this->m_head.field_C - 0.7f) * v34) * 0.75f) + (v33 * 1.2f));
		float v36 = this->m_armL.field_10 + bf10_times_2;
		float v37 = this->field_4 * 3.1416f;
		this->m_armL.field_10 = v36;
		this->m_armL.field_14 = Mth::sin(v37) * -0.4f;
	}
	if (this->field_10BE)
	{
		m_body.field_C = 0.5f;
		m_armL.field_C += 0.4f;
		m_armR.field_C += 0.4f;
		m_legL.m_posZ = 4.0f;
		m_legR.m_posZ = 4.0f;
		m_legL.m_posY = 9.0f;
		m_legR.m_posY = 9.0f;
		m_head.m_posY = 1.0f;
	}
	else
	{
		m_body.field_C = 0.0f;
		m_legL.m_posZ = 0.0f;
		m_head.m_posY = 0.0f;
		m_legR.m_posZ = 0.0f;
		m_legL.m_posY = 12.0f;
		m_legR.m_posY = 12.0f;
	}

	float v18 = this->m_armL.field_14;
	this->m_armL.field_14 = v18 + ((Mth::cos(a4 * 0.09f) * 0.05f) + 0.05f);
	float v19 = this->m_armR.field_14;
	this->m_armR.field_14 = v19 - ((Mth::cos(a4 * 0.09f) * 0.05f) + 0.05f);
	float v20 = this->m_armL.field_C;
	this->m_armL.field_C = v20 + (Mth::sin(a4 * 0.067f) * 0.05f);
	float v21 = this->m_armR.field_C;
	this->m_armR.field_C = v21 + (Mth::sin(a4 * 0.067f) * -0.05f);
}
*/
