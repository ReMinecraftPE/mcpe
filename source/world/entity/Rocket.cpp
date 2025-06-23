/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Rocket.hpp"
#include "Player.hpp"
#include "world/level/Level.hpp"

Rocket::Rocket(Level* level, const Vec3& pos) : Entity(level)
{
	field_B8C = 0;
	field_B90 = 80;
	field_C8 = RENDER_ROCKET;

    m_bBlocksBuilding = true;

	setSize(0.1f, 1.0f);
	m_heightOffset = m_bbHeight * 0.5f - 0.25f;

	setPos(pos);
	m_oPos = pos;

	m_vel.y = 1.0f;
}

float Rocket::getShadowHeightOffs() const
{
	return 0.0f;
}

bool Rocket::interact(Player* player)
{
	return true;
}

int Rocket::interactPreventDefault()
{
	return 1;
}

bool Rocket::isPickable() const
{
	return !m_bRemoved;
}

bool Rocket::isPushable() const
{
	return false;
}

void Rocket::tick()
{
	m_oPos = m_pos;

	m_vel.y *= 0.99f;

	move(m_vel);

	field_B90--;
	if (field_B90 == 0)
	{
		for (int i = 0; i < 100; i++)
		{
			float yaw = sharedRandom.nextFloat() * float(M_PI) * 2;
			float pitch = sharedRandom.nextFloat() * float(M_PI) * 2;

			Vec3 o(cosf(yaw), sinf(yaw), sinf(pitch));

			m_pLevel->addParticle("explodeColor", m_pos, o);
		}

		m_pLevel->playSound(this, "random.explode", 1.0f, 1.0f);

		remove();
		return;
	}
}
