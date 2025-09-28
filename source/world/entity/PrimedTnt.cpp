/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "PrimedTnt.hpp"
#include "world/level/Level.hpp"
#include "nbt/CompoundTag.hpp"

void PrimedTnt::_init()
{
	m_fuseTimer = 0;
	field_C8 = RENDER_TNT;
    m_bBlocksBuilding = true;
	setSize(0.98f, 0.98f);
	m_heightOffset = m_bbHeight * 0.5f;
	m_bMakeStepSound = false;
}

PrimedTnt::PrimedTnt(Level* level, const Vec3& pos) : Entity(level)
{
	_init();
	setPos(pos);

	float fAng = 0.017453f * 2.0f * float(M_PI) * Mth::random();

	m_vel.x = -0.02f * Mth::sin(fAng);
	m_vel.z = -0.02f * Mth::cos(fAng);
	m_vel.y = 0.2f;

	m_oPos = m_pos;

	m_fuseTimer = 80; // 4 secs
}

void PrimedTnt::explode()
{
	// @NOTE: Not passing `this` as pointer to entity
	m_pLevel->explode(nullptr, m_pos, 2.5f);
}

float PrimedTnt::getShadowHeightOffs() const
{
	return 0.0f;
}

bool PrimedTnt::isPickable() const
{
	return !m_bRemoved;
}

void PrimedTnt::tick()
{
	m_oPos = m_pos;

	m_vel.y -= 0.04f;
	move(m_vel);

	m_vel *= 0.98f;
	if (m_bOnGround)
	{
		m_vel.x *= 0.7f;
		m_vel.z *= 0.7f;
		m_vel.y *= -0.5f;
	}

	m_fuseTimer--;
	if (m_fuseTimer <= 0)
	{
		remove();
		explode();
	}
	else
	{
		m_pLevel->addParticle("smoke", Vec3(m_pos.x, m_pos.y + 0.5f, m_pos.z));
	}
}

void PrimedTnt::addAdditionalSaveData(CompoundTag& tag) const
{
	tag.putInt8("Fuse", m_fuseTimer);
}

void PrimedTnt::readAdditionalSaveData(const CompoundTag& tag)
{
	m_fuseTimer = tag.getInt8("Fuse");
}