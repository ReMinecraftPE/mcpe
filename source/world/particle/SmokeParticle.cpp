/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Particle.hpp"

SmokeParticle::SmokeParticle(Level* level, float x, float y, float z, float vx, float vy, float vz, float a9) :
	Particle(level, x, y, z, 0.0f, 0.0f, 0.0f)
{
	field_104 = 0.0f;

	m_vel.x = vx + m_vel.x * 0.1f;
	m_vel.y = vy + m_vel.y * 0.1f;
	m_vel.z = vz + m_vel.z * 0.1f;

	field_100 = field_FC = field_F8 = Mth::random() * 0.5f;

	field_104 = field_F0 = field_F0 * 0.75f * a9;

	m_bNoCollision = false;
	field_EC = int((a9 * 8.0f) / (0.2f + 0.8f * Mth::random()));
}

void SmokeParticle::render(Tesselator& t, float f, float a, float b, float c, float d, float e)
{
	float mult = 32.0f * (float(field_E8 + f) / float(field_EC));
	if (mult < 0.0f)
		mult = 0.0f;
	if (mult > 1.0f)
		mult = 1.0f;

	field_F0 = field_104 * mult;
	Particle::render(t, f, a, b, c, d, e);
}

void SmokeParticle::tick()
{
	field_3C = m_pos;
	
	field_E8++;
	if (field_E8 > field_EC)
		remove();

	m_vel.y += 0.004f;
	field_DC = -8 * field_E8 / field_EC + 7;

	move(m_vel.x, m_vel.y, m_vel.z);

	if (m_pos.y == field_3C.y)
	{
		m_vel.x *= 1.1f;
		m_vel.z *= 1.1f;
	}

	m_vel *= 0.96f;

	if (field_7C)
	{
		m_vel.x *= 0.7f;
		m_vel.z *= 0.7f;
	}
}
