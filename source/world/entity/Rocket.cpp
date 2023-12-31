/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Rocket.hpp"
#include "Player.hpp"
#include "world/level/Level.hpp"

Rocket::Rocket(Level* level, float x, float y, float z) : Entity(level)
{
	field_B8C = 0;
	field_B90 = 80;
	field_C8 = RENDER_CAMERA;

	field_34 = 1;

	setSize(1.0f, 1.5f);
	field_84 = field_8C * 0.5f - 0.25f;

	setPos(x, y, z);
	field_3C.x = x;
	field_3C.y = y;
	field_3C.z = z;

	m_vel.y = 1.0f;
}

float Rocket::getShadowHeightOffs()
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

bool Rocket::isPickable()
{
	return !m_bRemoved;
}

bool Rocket::isPushable()
{
	return false;
}

void Rocket::tick()
{
	field_3C = m_pos;

	m_vel.y *= 0.99f;

	move(m_vel.x, m_vel.y, m_vel.z);

	field_B90--;
	if (field_B90 == 0)
	{
		for (int i = 0; i < 50; i++)
		{
			float yaw = sharedRandom.nextFloat() * float(M_PI) * 2;
			float pitch = sharedRandom.nextFloat() * float(M_PI) * 2;

			float xo = sharedRandom.nextFloat() * cosf(yaw);
			float zo = sharedRandom.nextFloat() * sinf(yaw);
			float yo = sharedRandom.nextFloat() * sinf(pitch);

			m_pLevel->addParticle("explodeColor", m_pos.x, m_pos.y, m_pos.z, xo, yo, zo);
		}

		m_pLevel->playSound(this, "random.explode", 1.0f, 1.0f);

		remove();
		return;
	}
}
