/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Particle.hpp"
#include "world/level/Level.hpp"

BubbleParticle::BubbleParticle(Level* level, float x, float y, float z, float vx, float vy, float vz) :
	Particle(level, x, y, z, vx, vy, vz)
{
	field_F8 = field_FC = field_100 = 1.0f;
	field_DC = PTI_BUBBLE;
	setSize(0.02f, 0.02f);

	field_F0 *= 0.2f + 0.6f * sharedRandom.nextFloat();
	m_vel.x = vx * 0.2f + 0.02f * (2.0f * Mth::random() - 1.0f);
	m_vel.y = vy * 0.2f + 0.02f * (2.0f * Mth::random() - 1.0f);
	m_vel.z = vz * 0.2f + 0.02f * (2.0f * Mth::random() - 1.0f);
	field_EC = int(8.0f / (Mth::random() * 0.8f + 0.2f));
}

void BubbleParticle::tick()
{
	field_3C = m_pos;

	m_vel.y += 0.002f;
	move(m_vel.x, m_vel.y, m_vel.z);

	m_vel *= 0.85f;

	if (m_pLevel->getMaterial(Mth::floor(m_pos.x), Mth::floor(m_pos.y), Mth::floor(m_pos.z)) != Material::water)
		remove();

	field_EC--;
	if (field_EC <= 0)
		remove();
}
