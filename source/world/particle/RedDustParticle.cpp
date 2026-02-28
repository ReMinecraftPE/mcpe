/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Particle.hpp"

RedDustParticle::RedDustParticle(Level* level, const Vec3& pos, const Vec3& dir) :
	Particle(level, pos, Vec3::ZERO)
{
	m_oSize = 0.0f;

	m_vel = dir + m_vel * 0.1f;

	float f = Mth::random() * 0.4f + 0.6f;
	m_rCol  = f * (dir.x != 0.0f ? dir.x : 1.0f) * (Mth::random() * 0.2f + 0.8f);
	m_gCol  = f * dir.y						   * (Mth::random() * 0.2f + 0.8f);
	m_bCol = f * dir.z						   * (Mth::random() * 0.2f + 0.8f);

	m_oSize = m_size = m_size * 0.75f;

	m_bNoPhysics = false;
	m_lifetime = int(8.0f / (0.2f + 0.8f * Mth::random()));
}

void RedDustParticle::render(Tesselator& t, float f, float a, float b, float c, float d, float e)
{
	float mult = 32.0f * (float(m_age + f) / float(m_lifetime));
	if (mult < 0.0f)
		mult = 0.0f;
	if (mult > 1.0f)
		mult = 1.0f;

	m_size = m_oSize * mult;
	Particle::render(t, f, a, b, c, d, e);
}

void RedDustParticle::tick()
{
	m_oPos = m_pos;

	m_age++;
	if (m_age > m_lifetime)
		remove();

	m_vel.y += 0.004f;
	m_tex = -8 * m_age / m_lifetime + 7;

	move(m_vel);

	if (m_pos.y == m_oPos.y)
	{
		m_vel.x *= 1.1f;
		m_vel.z *= 1.1f;
	}

	m_vel *= 0.96f;

	if (m_bOnGround)
	{
		m_vel.x *= 0.7f;
		m_vel.z *= 0.7f;
	}
}
