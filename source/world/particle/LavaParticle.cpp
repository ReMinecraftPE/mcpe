/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Particle.hpp"
#include "world/level/Level.hpp"

LavaParticle::LavaParticle(Level* level, const Vec3& pos) :
	Particle(level, pos, Vec3::ZERO)
{
	field_104 = 0.0f;

	m_vel *= 0.8f;
	m_vel.y = sharedRandom.nextFloat() * 0.4f + 0.05f;
	m_rCol = m_gCol = m_bCol = 1.0f;
	field_104 = m_size = m_size * (0.2f + 2 * sharedRandom.nextFloat());
	m_tex = PTI_LAVA;
	m_lifetime = int(16.0f / (0.2f + 0.8f * Mth::random()));
}

float LavaParticle::getBrightness(float unused) const
{
	return 1.0f;
}

void LavaParticle::tick()
{
	m_oPos = m_pos;

	m_timer++;
	if (m_timer > m_lifetime)
		remove();

	float a = float(m_timer) / float(m_lifetime);
	float b = sharedRandom.nextFloat();
	if (a < b)
	{
		m_pLevel->addParticle("smoke", m_pos, m_vel);
	}

	m_vel.y -= 0.03f;
	move(m_vel);

	m_vel *= 0.999f;
	if (m_bOnGround)
	{
		m_vel.x *= 0.7f;
		m_vel.z *= 0.7f;
	}
}

void LavaParticle::render(Tesselator& t, float f, float a, float b, float c, float d, float e)
{
	float mult = float(m_timer + f) / float(m_lifetime);
	m_size = field_104 * (1.0f - mult * mult);
	Particle::render(t, f, a, b, c, d, e);
}
