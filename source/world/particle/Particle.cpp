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
	field_DC = 0;
	field_E0 = 0.0f;
	field_E4 = 0.0f;
	field_E8 = 0;
	field_EC = 0;
	field_F0 = 0.0f;
	field_F4 = 0.0f;
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

	field_E0 = 3.0f * sharedRandom.nextFloat();
	field_E4 = 3.0f * sharedRandom.nextFloat();
	field_F0 = 2.0f * (0.5f + 0.5f * sharedRandom.nextFloat());
	field_EC = int(4.0f / (0.1f + 0.9f * sharedRandom.nextFloat()));
}

int Particle::getParticleTexture()
{
	return PT_PARTICLES;
}

Particle* Particle::scale(float f)
{
	setSize(0.2f * f, 0.2f * f);
	field_F0 *= f;
	return this;
}

Particle* Particle::setPower(float f)
{
	m_vel.x *= f;
	m_vel.y = (m_vel.y - 0.1f) * f + 0.1f;
	m_vel.z *= f;
	return this;
}

void Particle::render(Tesselator& t, float f, float a4, float a5, float a6, float a7, float a8)
{
	constexpr float C_MAGIC_1 = 0.062438f; // @BUG: Slightly bigger than 1/16.0f

	int texture = field_DC;
	int texX = texture % 16;
	if (texture < 0)
		texture += 15;

	float texU_1 = float(texX) / 16.0f;
	float texV_1 = float(texture >> 4) / 16.0f;

	float posX = Mth::Lerp(m_oPos.x, m_pos.x, f) - xOff;
	float posY = Mth::Lerp(m_oPos.y, m_pos.y, f) - yOff;
	float posZ = Mth::Lerp(m_oPos.z, m_pos.z, f) - zOff;
	float fBright = m_bIsUnlit ? 1.0f : getBrightness(f);

	float sizeX = a4 * field_F0 * 0.1f;
	float sizeY = a5 * field_F0 * 0.1f;
	float sizeZ = a6 * field_F0 * 0.1f;
	float siz2X = a7 * field_F0 * 0.1f;
	float siz2Z = a8 * field_F0 * 0.1f;

	t.color(m_rCol * fBright, m_gCol * fBright, m_bCol * fBright);
	t.vertexUV(posX - sizeX - siz2X, posY - sizeY, posZ - sizeZ - siz2Z, texU_1 + C_MAGIC_1, texV_1 + C_MAGIC_1);
	t.vertexUV(posX - sizeX + siz2X, posY + sizeY, posZ - sizeZ + siz2Z, texU_1 + C_MAGIC_1, texV_1);
	t.vertexUV(posX + sizeX + siz2X, posY + sizeY, posZ + sizeZ + siz2Z, texU_1,             texV_1);
	t.vertexUV(posX + sizeX - siz2X, posY - sizeY, posZ + sizeZ - siz2Z, texU_1,             texV_1 + C_MAGIC_1);
}

void Particle::tick()
{
	m_oPos = m_pos;
	field_E8++;
	if (field_E8 >= field_EC)
		remove();
	
	m_vel.y -= field_F4 * 0.04f;
	move(m_vel);

	m_vel *= 0.98f;

	if (m_bOnGround)
	{
		m_vel.x *= 0.7f;
		m_vel.z *= 0.7f;
	}
}
