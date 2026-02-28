/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Particle.hpp"

float Particle::xOff, Particle::yOff, Particle::zOff;

void Particle::_init()
{
	m_tex = 0;
	m_uo = 0.0f;
	m_vo = 0.0f;
	m_age = 0;
	m_lifetime = 0;
	m_size = 0.0f;
	m_gravity = 0.0f;
	m_rCol = 1.0f;
	m_gCol = 1.0f;
	m_bCol = 1.0f;
	m_bIsUnlit = false;
	m_bMakeStepSound = false;
}

Particle::Particle(Level* level, const Vec3& pos, const Vec3& dir) : Entity(level)
{
	_init();

	setSize(0.2f, 0.2f);
	m_heightOffset = 0.5f * m_bbHeight;
	
	setPos(pos);
	m_vel.x = dir.x + 0.4f * (2.0f * Mth::random() - 1.0f);
	m_vel.y = dir.y + 0.4f * (2.0f * Mth::random() - 1.0f);
	m_vel.z = dir.z + 0.4f * (2.0f * Mth::random() - 1.0f);

	float mult = 0.4f * 0.15f * (Mth::random() + Mth::random() + 1.0f) / m_vel.length();
	m_vel.x *= mult;
	m_vel.y = m_vel.y * mult + 0.1f;
	m_vel.z *= mult;

	m_uo = 3.0f * sharedRandom.nextFloat();
	m_vo = 3.0f * sharedRandom.nextFloat();
	m_size = 2.0f * (0.5f + 0.5f * sharedRandom.nextFloat());
	m_lifetime = int(4.0f / (0.1f + 0.9f * sharedRandom.nextFloat()));
}

int Particle::getParticleTexture()
{
	return PT_PARTICLES;
}

Particle* Particle::scale(float f)
{
	setSize(0.2f * f, 0.2f * f);
	m_size *= f;
	return this;
}

Particle* Particle::setPower(float f)
{
	m_vel.x *= f;
	m_vel.y = (m_vel.y - 0.1f) * f + 0.1f;
	m_vel.z *= f;
	return this;
}

void Particle::render(Tesselator& t, float f, float xa, float ya, float za, float xa2, float za2)
{
	constexpr float C_MAGIC_1 = 0.062438f; // @BUG: Slightly bigger than 1/16.0f

	int texture = m_tex;
	int texX = texture % 16;
	if (texture < 0)
		texture += 15;

	float texU_1 = float(texX) / 16.0f;
	float texV_1 = float(texture >> 4) / 16.0f;

	float posX = Mth::Lerp(m_oPos.x, m_pos.x, f) - xOff;
	float posY = Mth::Lerp(m_oPos.y, m_pos.y, f) - yOff;
	float posZ = Mth::Lerp(m_oPos.z, m_pos.z, f) - zOff;
	float fBright = m_bIsUnlit ? 1.0f : getBrightness(f);

	float sizeX = xa * m_size * 0.1f;
	float sizeY = ya * m_size * 0.1f;
	float sizeZ = za * m_size * 0.1f;
	float siz2X = xa2 * m_size * 0.1f;
	float siz2Z = za2 * m_size * 0.1f;

	t.color(m_rCol * fBright, m_gCol * fBright, m_bCol * fBright);
	t.vertexUV(posX - sizeX - siz2X, posY - sizeY, posZ - sizeZ - siz2Z, texU_1 + C_MAGIC_1, texV_1 + C_MAGIC_1);
	t.vertexUV(posX - sizeX + siz2X, posY + sizeY, posZ - sizeZ + siz2Z, texU_1 + C_MAGIC_1, texV_1);
	t.vertexUV(posX + sizeX + siz2X, posY + sizeY, posZ + sizeZ + siz2Z, texU_1,             texV_1);
	t.vertexUV(posX + sizeX - siz2X, posY - sizeY, posZ + sizeZ - siz2Z, texU_1,             texV_1 + C_MAGIC_1);
}

void Particle::tick()
{
	m_oPos = m_pos;
	m_age++;
	if (m_age >= m_lifetime)
		remove();
	
	m_vel.y -= m_gravity * 0.04f;
	move(m_vel);

	m_vel *= 0.98f;

	if (m_bOnGround)
	{
		m_vel.x *= 0.7f;
		m_vel.z *= 0.7f;
	}
}
