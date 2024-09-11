/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Particle.hpp"
#include "world/level/Level.hpp"

BubbleParticle::BubbleParticle(Level* level, const Vec3& pos, const Vec3& dir) :
	Particle(level, pos, dir)
{
	m_rCol = m_gCol = m_bCol = 1.0f;
	field_DC = PTI_BUBBLE;
	setSize(0.02f, 0.02f);

	field_F0 *= 0.2f + 0.6f * sharedRandom.nextFloat();
	m_vel.x = dir.x * 0.2f + 0.02f * (2.0f * Mth::random() - 1.0f);
	m_vel.y = dir.y * 0.2f + 0.02f * (2.0f * Mth::random() - 1.0f);
	m_vel.z = dir.z * 0.2f + 0.02f * (2.0f * Mth::random() - 1.0f);
	field_EC = int(8.0f / (Mth::random() * 0.8f + 0.2f));
}

void BubbleParticle::tick()
{
	m_ySlideOffset = m_pos;

	m_vel.y += 0.002f;
	move(m_vel);

	m_vel *= 0.85f;

	if (m_pLevel->getMaterial(m_pos) != Material::water)
		remove();

	field_EC--;
	if (field_EC <= 0)
		remove();
}
