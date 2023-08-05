/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Particle.hpp"

FlameParticle::FlameParticle(Level* level, float x, float y, float z, float vx, float vy, float vz) :
	Particle(level, x, y, z, vx, vy, vz)
{
	m_vel.x = m_vel.x * 0.01f + vx;
	m_vel.y = m_vel.y * 0.01f + vy;
	m_vel.z = m_vel.z * 0.01f + vz;

	// @NOTE: Useless genrand_int32 calls. Will keep them in to keep consistent
	sharedRandom.genrand_int32();
	sharedRandom.genrand_int32();
	sharedRandom.genrand_int32();
	sharedRandom.genrand_int32();
	sharedRandom.genrand_int32();
	sharedRandom.genrand_int32();

	field_104 = field_F0;
	field_F8 = field_FC = field_100 = 1.0f;
	field_EC = int(8.0f / (0.2f + 0.8f * Mth::random())) + 4;
	field_DC = PTI_FLAME;
}

float FlameParticle::getBrightness(float unused)
{
	return 1.0f;
}

void FlameParticle::tick()
{
	field_3C = m_pos;

	field_E8++;
	if (field_E8 > field_EC)
		remove();

	move(m_vel.x, m_vel.y, m_vel.z);

	m_vel *= 0.96f;

	if (field_7C)
	{
		m_vel.x *= 0.7f;
		m_vel.z *= 0.7f;
	}
}

void FlameParticle::render(Tesselator& t, float f, float a, float b, float c, float d, float e)
{
	float mult = float(field_E8 + f) / float(field_EC);
	field_F0 = field_104 * (1.0f - 0.5f * mult * mult);
	Particle::render(t, f, a, b, c, d, e);
}
