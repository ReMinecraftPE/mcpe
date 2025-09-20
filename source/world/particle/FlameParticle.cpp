/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Particle.hpp"

FlameParticle::FlameParticle(Level* level, const Vec3& pos, const Vec3& dir) :
	Particle(level, pos, dir)
{
	field_104 = 0.0f;

	m_vel = m_vel * 0.01f + dir;

	// @NOTE: Useless genrand_int32 calls. Will keep them in to keep consistent
	sharedRandom.genrand_int32();
	sharedRandom.genrand_int32();
	sharedRandom.genrand_int32();
	sharedRandom.genrand_int32();
	sharedRandom.genrand_int32();
	sharedRandom.genrand_int32();

	field_104 = field_F0;
	m_rCol = m_gCol = m_bCol = 1.0f;
	field_EC = int(8.0f / (0.2f + 0.8f * Mth::random())) + 4;
	field_DC = PTI_FLAME;
}

float FlameParticle::getBrightness(float unused) const
{
	return 1.0f;
}

void FlameParticle::tick()
{
	m_oPos = m_pos;

	field_E8++;
	if (field_E8 > field_EC)
		remove();

	move(m_vel);

	m_vel *= 0.96f;

	if (m_bOnGround)
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
