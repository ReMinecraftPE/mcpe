/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "PrimedTnt.hpp"
#include "Level.hpp"

PrimedTnt::PrimedTnt(Level* level) : Entity(level)
{
	m_fuseTimer = 0;
	field_C8 = RENDER_TNT;
	field_34 = 1;
	setSize(0.98f, 0.98f);
	field_84 = field_8C * 0.5f;
}

PrimedTnt::PrimedTnt(Level* level, float x, float y, float z) : PrimedTnt(level)
{
	setPos(x, y, z);

	float fAng = 0.017453f * 2.0f * float(M_PI) * Mth::random();

	m_vel.x = -0.02f * Mth::sin(fAng);
	m_vel.z = -0.02f * Mth::cos(fAng);
	m_vel.y = 0.2f;

	field_3C = m_pos;

	m_fuseTimer = 80; // 4 secs
}

void PrimedTnt::explode()
{
	// @NOTE: Not passing `this` as pointer to entity
	m_pLevel->explode(nullptr, m_pos.x, m_pos.y, m_pos.z, 2.5f);
}

float PrimedTnt::getShadowHeightOffs()
{
	return 0.0f;
}

bool PrimedTnt::isPickable()
{
	return !m_bRemoved;
}

void PrimedTnt::tick()
{
	field_3C = m_pos;

	m_vel.y -= 0.04f;
	move(m_vel.x, m_vel.y, m_vel.z);

	m_vel *= 0.98f;
	if (field_7C)
	{
		m_vel.x *= 0.7f;
		m_vel.z *= 0.7f;
		m_vel.y *= -0.5f;
	}

	m_fuseTimer--;
	if (m_fuseTimer <= 0)
	{
		remove();
		explode();
	}
	else
	{
		m_pLevel->addParticle("smoke", m_pos.x, m_pos.y + 0.5f, m_pos.z, 0.0f, 0.0f, 0.0f);
	}
}
