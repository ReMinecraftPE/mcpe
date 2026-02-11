/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023-2024 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "HumanoidModel.hpp"
#include "common/Mth.hpp"
#include "client/app/Minecraft.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"

HumanoidModel::HumanoidModel(float modelSize, float yOffset) :
	Model(64, 32),
	m_head(this, 0,  0),
	m_hat(this, 32, 0),
	m_body(this, 16, 16),
	m_armRight(this, 40, 16),
	m_armLeft(this, 40, 16),
	m_legRight(this, 0,  16),
	m_legLeft(this, 0,  16)
{
	field_20 = false;
	m_bHoldingLeftHand = false;
	m_bHoldingRightHand = false;
	m_bSneaking = false;
	field_237 = false;

	m_head.addBox(-4, -8, -4, 8, 8, 8, modelSize);
	m_head.setPos(0, yOffset, 0);

	m_hat.addBox(-4, -8, -4, 8, 8, 8, modelSize + 0.5f);
	m_hat.setPos(0, yOffset, 0);

	m_body.addBox(-4, 0, -2, 8, 12, 4, modelSize);
	m_body.setPos(0, yOffset, 0);

	m_armRight.addBox(-3, -2, -2, 4, 12, 4, modelSize);
	m_armRight.setPos(-5, yOffset + 2, 0);

	m_armLeft.m_bMirror = true;
	m_armLeft.addBox(-1, -2, -2, 4, 12, 4, modelSize);
	m_armLeft.setPos(5, yOffset + 2, 0);

	m_legRight.addBox(-2, 0, -2, 4, 12, 4, modelSize);
	m_legRight.setPos(-2, yOffset + 12, 0);

	m_legLeft.m_bMirror = true;
	m_legLeft.addBox(-2, 0, -2, 4, 12, 4, modelSize);
	m_legLeft.setPos(2, yOffset + 12, 0);
}

void HumanoidModel::onGraphicsReset()
{
	m_head.m_bCompiled = false;
	m_body.m_bCompiled = false;
	m_armRight.m_bCompiled = false;
	m_armLeft.m_bCompiled = false;
	m_legRight.m_bCompiled = false;
	m_legLeft.m_bCompiled = false;
}

void HumanoidModel::render(float a, float b, float c, float d, float e, float f)
{
	setupAnim(a, b, c, d, e, f);
	m_head.render(f);
	m_body.render(f);
	m_armRight.render(f);
	m_armLeft.render(f);
	m_legRight.render(f);
	m_legLeft.render(f);
	m_hat.m_rot.y = m_head.m_rot.y;
	m_hat.m_rot.x = m_head.m_rot.x;
	m_hat.render(f, &m_materials.entity_alphatest);
}

void HumanoidModel::setupAnim(float a2, float a3, float a4, float a5, float a6, float a7)
{
	//a6 = pitch, a5 = yaw-ish
	m_head.m_rot.y = a5 * MTH_DEG_TO_RAD;
	m_head.m_rot.x = a6 * MTH_DEG_TO_RAD;
	m_hat.m_rot.y = m_head.m_rot.y;
	m_hat.m_rot.y = m_head.m_rot.x;
	if (m_head.m_rot.x < -1.0f)
		m_head.m_rot.x = -1.0f;
	if (m_head.m_rot.x > 1.0f)
		m_head.m_rot.x = 1.0f;
	float v12 = (a2 * 0.6662f) + 3.1416f;
	m_armRight.m_rot.x = (Mth::cos(v12) * 2.0f * a3) * 0.5f;
	m_armLeft.m_rot.x = Mth::cos(a2 * 0.6662f) * 2.0f * a3 * 0.5f; // @HUH: multiplying by 2 and also by 1/2
	m_armRight.m_rot.z = 0.0f;
	m_armLeft.m_rot.z = 0.0f;
	m_legRight.m_rot.x = (Mth::cos(a2 * 0.6662f) * 1.4f) * a3;
	m_legLeft.m_rot.x = (Mth::cos(v12) * 1.4f) * a3;
	m_legRight.m_rot.y = 0.0f;
	m_legLeft.m_rot.y = 0.0f;
	
	if (m_bRiding)
	{
		float v15 = (3.1416f * -0.5f) * 0.4f;
		m_armRight.m_rot.x += v15;
		m_armLeft.m_rot.x += v15;
		float v16 = (3.1416f * -0.5f) * 0.8f;
		m_legRight.m_rot.x = v16;
		m_legLeft.m_rot.x = v16;
		m_legRight.m_rot.y = (3.1416f * 0.5f) * 0.2f;
		m_legLeft.m_rot.y = (3.1416f * -0.5f) * 0.2f;
	}

	if (m_bHoldingLeftHand)
		m_armLeft.m_rot.x = ((3.1416f * 0.5f) * -0.2f) + (m_armLeft.m_rot.x * 0.5f);
	if (m_bHoldingRightHand)
		m_armRight.m_rot.x = ((3.1416f * 0.5f) * -0.2f) + (m_armRight.m_rot.x * 0.5f);
	
	m_armRight.m_rot.y = 0.0f;
	m_armLeft.m_rot.y = 0.0f;

	if (m_attackTime > -9990.0f)
	{
		m_body.m_rot.y = Mth::sin(Mth::sqrt(m_attackTime) * 3.1416f * 2.0f) * 0.2f;
		m_armRight.m_pos.z = 5.0f * Mth::sin(m_body.m_rot.y);
		m_armRight.m_pos.x = -5.0f * Mth::cos(m_body.m_rot.y);
		m_armLeft.m_pos.z = -5.0f * Mth::sin(m_body.m_rot.y);
		m_armLeft.m_pos.x = 5.0f * Mth::cos(m_body.m_rot.y);
		m_armRight.m_rot.y = m_armRight.m_rot.y + m_body.m_rot.y;
		m_armLeft.m_rot.y = m_armLeft.m_rot.y + m_body.m_rot.y;
		float o = 1.0f - m_attackTime;
		m_armLeft.m_rot.x += m_body.m_rot.y;
		m_armRight.m_rot.x -= -((m_head.m_rot.x - 0.7f) * Mth::sin(3.1416f * m_attackTime)) * 0.75f + Mth::sin((1.0f - o * o * o * o) * 3.1416f) * 1.2f;
		m_armRight.m_rot.y += m_body.m_rot.y * 2.0f;
		m_armRight.m_rot.z = Mth::sin(m_attackTime * 3.1416f) * -0.4f;
	}

	if (m_bSneaking)
	{
		m_body.m_rot.x = 0.5f;
		m_armRight.m_rot.x += 0.4f;
		m_armLeft.m_rot.x += 0.4f;
		m_legRight.m_pos.z = 4.0f;
		m_legLeft.m_pos.z = 4.0f;
		m_legRight.m_pos.y = 9.0f;
		m_legLeft.m_pos.y = 9.0f;
		m_head.m_pos.y = 1.0f;
		m_hat.m_pos.y = 1.0f;
	}
	else
	{
		m_body.m_rot.x = 0.0f;
		m_legRight.m_pos.z = 0.0f;
		m_head.m_pos.y = 0.0f;
		m_hat.m_pos.y = 0.0f;
		m_legLeft.m_pos.z = 0.0f;
		m_legRight.m_pos.y = 12.0f;
		m_legLeft.m_pos.y = 12.0f;
	}

	m_armRight.m_rot.z += Mth::cos(a4 * 0.09f) * 0.05f + 0.05f;
	m_armLeft.m_rot.z -= Mth::cos(a4 * 0.09f) * 0.05f + 0.05f;
	m_armRight.m_rot.x += Mth::sin(a4 * 0.067f) * 0.05f;
	m_armLeft.m_rot.x += Mth::sin(a4 * 0.067f) * -0.05f;
}

void HumanoidModel::setBrightness(float f)
{
	m_armRight.setBrightness(f);
	m_armLeft.setBrightness(f);
	m_legRight.setBrightness(f);
	m_legLeft.setBrightness(f);
	m_body.setBrightness(f);
	m_head.setBrightness(f);
	m_hat.setBrightness(f);
}
