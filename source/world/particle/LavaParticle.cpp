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
	field_104 = field_F0 = field_F0 * (0.2f + 2 * sharedRandom.nextFloat());
	field_DC = PTI_LAVA;
	field_EC = int(16.0f / (0.2f + 0.8f * Mth::random()));
}

float LavaParticle::getBrightness(float unused) const
{
	return 1.0f;
}

void LavaParticle::tick()
{
	m_oPos = m_pos;

	field_E8++;
	if (field_E8 > field_EC)
		remove();

	float a = float(field_E8) / float(field_EC);
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
	float mult = float(field_E8 + f) / float(field_EC);
	field_F0 = field_104 * (1.0f - mult * mult);
	Particle::render(t, f, a, b, c, d, e);
}
