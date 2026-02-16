#include "Particle.hpp"

NoteParticle::NoteParticle(Level* level, const Vec3& pos, const Vec3& dir, float scale) :
	Particle(level, pos, Vec3::ZERO)
{
	m_vel.x *= 0.01f;
	m_vel.y *= 0.01f;
	m_vel.z *= 0.01f;
	m_vel.y += 0.2f;
	m_rCol = Mth::sin((dir.x + 0.0f) * M_PI * 2.0f) * 0.65f + 0.35f;
	m_gCol = Mth::sin((dir.x + (1.0f / 3.0f)) * M_PI * 2.0f) * 0.65f + 0.35f;
	m_bCol = Mth::sin((dir.x + (2.0f / 3.0f)) * M_PI * 2.0f) * 0.65f + 0.35f;
	m_size *= 0.75f * scale;
	m_oSize = m_size;
	m_lifetime = 6;
	m_bNoPhysics = false;
	m_tex = PTI_NOTE;
}

void NoteParticle::tick()
{
	m_oPos = m_pos;

	m_timer++;
	if (m_timer > m_lifetime)
		remove();

	move(m_vel);
	if (m_pos.y == m_oPos.y)
	{
		m_vel *= Vec3(1.1f, 1.0f, 1.1f);
	}

	m_vel *= 0.66f;

	if (m_bOnGround)
	{
		m_vel *= Vec3(0.7f, 1.0f, 0.7f);
	}
}

void NoteParticle::render(Tesselator& t, float f, float a, float b, float c, float d, float e)
{
	float mult = float(m_timer + f) / float(m_lifetime) * 32.0f;
	m_size = m_oSize * Mth::clamp(mult, 0.0f, 1.0f);
	Particle::render(t, f, a, b, c, d, e);
}
