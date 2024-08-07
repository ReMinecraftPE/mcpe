/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TripodCamera.hpp"
#include "Player.hpp"
#include "world/level/Level.hpp"

TripodCamera::TripodCamera(Level* level, Player* player, const Vec3& pos) : Mob(level)
{
	field_B8C = 0;
	field_B90 = 80;
	m_bActive = false;

	m_owner = player;
	field_C8 = RENDER_CAMERA;

	m_rotPrev = m_rot = player->m_rot;

	field_34 = 1;

	setSize(1.0f, 1.5f);
	m_heightOffset = field_8C * 0.5f - 0.25f;

	setPos(pos);
	field_3C = pos;
}

void TripodCamera::defineSynchedData()
{

}

bool TripodCamera::interact(Player* player)
{
	// @BUG-ish: No check for owner?
	m_bActive = true;
	return true;
}

int TripodCamera::interactPreventDefault()
{
	return 1;
}

void TripodCamera::tick()
{
	field_3C = m_pos;

	m_vel.y -= 0.04f;
	move(m_vel);

	m_vel *= 0.98f;
	if (m_onGround)
	{
		m_vel.x *= 0.7f;
		m_vel.z *= 0.7f;
		m_vel.y *= -0.5f;
	}

	if (!m_bActive)
		return;

	field_B90--;
	if (field_B90 == 0)
	{
		remove();
		return;
	}

	if (field_B90 == 8)
	{
		m_pLevel->takePicture(this, m_owner);
		m_pLevel->addParticle("explode", Vec3(m_pos.x, m_pos.y + 0.6f, m_pos.z));
		m_pLevel->addParticle("explode", Vec3(m_pos.x, m_pos.y + 0.8f, m_pos.z));
		m_pLevel->addParticle("explode", Vec3(m_pos.x, m_pos.y + 1.0f, m_pos.z));
		return;
	}

	if (field_B90 > 8)
	{
		m_pLevel->addParticle("smoke", Vec3(m_pos.x, m_pos.y + 1.0f, m_pos.z));
	}
}
