/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Particle.hpp"
#include "world/level/Level.hpp"

LavaParticle::LavaParticle(Level* level, float x, float y, float z) :
	Particle(level, x, y, z, 0.0f, 0.0f, 0.0f)
{
	m_vel *= 0.8f;
	m_vel.y = sharedRandom.nextFloat() * 0.4f + 0.05f;
	field_F8 = field_FC = field_100 = 1.0f;
	field_104 = field_F0 = field_F0 * (0.2f + 2 * sharedRandom.nextFloat());
	field_DC = PTI_LAVA;
	field_EC = int(16.0f / (0.2f + 0.8f * Mth::random()));
}

float LavaParticle::getBrightness(float unused)
{
	return 1.0f;
}

void LavaParticle::tick()
{
	field_3C = m_pos;

	field_E8++;
	if (field_E8 > field_EC)
		remove();

	float a = float(field_E8) / float(field_EC);
	float b = sharedRandom.nextFloat();
	if (a < b)
	{
		m_pLevel->addParticle("smoke", m_pos.x, m_pos.y, m_pos.z, m_vel.x, m_vel.y, m_vel.z);
	}

	m_vel.y -= 0.03f;
	move(m_vel.x, m_vel.y, m_vel.z);

	m_vel *= 0.999f;
	if (field_7C)
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
