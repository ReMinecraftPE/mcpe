/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Particle.hpp"

ExplodeParticle::ExplodeParticle(Level* level, const Vec3& pos, const Vec3& dir) :
	Particle(level, pos, dir)
{
	field_104 = 0.0f;

	m_vel.x = dir.x + 0.05f * (2.0f * Mth::random() - 1.0f);
	m_vel.y = dir.y + 0.05f * (2.0f * Mth::random() - 1.0f);
	m_vel.z = dir.z + 0.05f * (2.0f * Mth::random() - 1.0f);

	m_rCol = m_gCol = m_bCol = 0.7f + 0.3f * sharedRandom.nextFloat();
	m_size = 1.0f + 6.0f * sharedRandom.nextFloat() * sharedRandom.nextFloat();
	m_lifetime = int(16.0f / (0.2f + 0.8f * sharedRandom.nextFloat())) + 2;
}

void ExplodeParticle::tick()
{
	m_oPos = m_pos;
	
	m_timer++;
	if (m_timer > m_lifetime)
		remove();

	m_vel.y += 0.004f;
	m_tex = -8 * m_timer / m_lifetime + 7;

	move(m_vel);

	m_vel *= 0.9f;

	if (m_bOnGround)
	{
		m_vel.x *= 0.7f;
		m_vel.z *= 0.7f;
	}
}
