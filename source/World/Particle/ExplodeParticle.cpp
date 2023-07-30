/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Particle.hpp"

ExplodeParticle::ExplodeParticle(Level* level, float x, float y, float z, float vx, float vy, float vz) :
	Particle(level, x, y, z, vx, vy, vz)
{
	m_vel.x = vx + 0.05f * (2.0f * Mth::random() - 1.0f);
	m_vel.y = vy + 0.05f * (2.0f * Mth::random() - 1.0f);
	m_vel.z = vz + 0.05f * (2.0f * Mth::random() - 1.0f);

	field_F8 = field_FC = field_100 = 0.7f + 0.3f * sharedRandom.nextFloat();
	field_F0 = 1.0f + 6.0f * sharedRandom.nextFloat() * sharedRandom.nextFloat();
	field_EC = int(16.0f / (0.2f + 0.8f * sharedRandom.nextFloat())) + 2;
}

void ExplodeParticle::tick()
{
	field_3C = m_pos;
	
	field_E8++;
	if (field_E8 > field_EC)
		remove();

	m_vel.y += 0.004f;
	field_DC = -8 * field_E8 / field_EC + 7;

	move(m_vel.x, m_vel.y, m_vel.z);

	m_vel *= 0.9f;

	if (field_7C)
	{
		m_vel.x *= 0.7f;
		m_vel.z *= 0.7f;
	}
}
