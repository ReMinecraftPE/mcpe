/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TripodCamera.hpp"
#include "Player.hpp"
#include "Level.hpp"

TripodCamera::TripodCamera(Level* level, Player* player, float x, float y, float z) : Mob(level)
{
	m_owner = player;
	field_C8 = RENDER_CAMERA;

	field_60 = m_pitch = player->m_pitch;
	field_5C = m_yaw   = player->m_yaw;

	field_34 = 1;

	setSize(1.0f, 1.5f);
	field_84 = field_8C * 0.5f - 0.25f;
	
	setPos(x, y, z);
	field_3C.x = x;
	field_3C.y = y;
	field_3C.z = z;
}

void TripodCamera::defineSynchedData()
{

}

float TripodCamera::getShadowHeightOffs()
{
	return 0.0f;
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

bool TripodCamera::isPickable()
{
	return !m_bRemoved;
}

bool TripodCamera::isPushable()
{
	return false;
}

void TripodCamera::tick()
{
	field_3C = m_pos;

	m_vel.y -= 0.04f;
	move(m_vel.x, m_vel.y, m_vel.z);

	m_vel *= 0.98f;
	if (field_7C)
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
		m_pLevel->addParticle("explode", m_pos.x, m_pos.y + 0.6f, m_pos.z, 0.0f, 0.0f, 0.0f);
		m_pLevel->addParticle("explode", m_pos.x, m_pos.y + 0.8f, m_pos.z, 0.0f, 0.0f, 0.0f);
		m_pLevel->addParticle("explode", m_pos.x, m_pos.y + 1.0f, m_pos.z, 0.0f, 0.0f, 0.0f);
		return;
	}

	if (field_B90 > 8)
	{
		m_pLevel->addParticle("smoke", m_pos.x, m_pos.y + 1.0f, m_pos.z, 0.0f, 0.0f, 0.0f);
	}
}
