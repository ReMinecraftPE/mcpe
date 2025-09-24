#include "Spider.hpp"

Spider::Spider(Level* pLevel) : Monster(pLevel)
{
	m_pDescriptor = &EntityTypeDescriptor::spider;
	field_C8 = RENDER_SPIDER;

	m_texture = "mob/spider.png";
	setSize(1.4f, 0.9f);
	m_runSpeed = 0.8f;
}

Entity* Spider::findAttackTarget()
{
	if (getBrightness(1.0f) < 0.5f)
		return m_pLevel->getNearestAttackablePlayer(*this, 16.0f);

	return nullptr;
}

void Spider::checkHurtTarget(Entity* ent, float var2)
{
	float brightness = getBrightness(1.0f);

	if (brightness > 0.5f && m_random.nextInt(100) == 0)
	{
		m_pAttackTarget = nullptr;
	}
	else
	{
		if (var2 > 2.0f && var2 < 6.0f && m_random.nextInt(10) == 0)
		{
			if (m_bOnGround)
			{
				float var4 = ent->m_pos.x - m_pos.x;
				float var6 = ent->m_pos.z - m_pos.z;
				float var8 = Mth::sqrt(var4 * var4 + var6 * var6);
				m_vel.x = var4 / var8 * 0.5f * 0.8f + m_vel.x * 0.2f;
				m_vel.z = var6 / var8 * 0.5f * 0.8f + m_vel.z * 0.2f;
				m_vel.y = 0.4f;
			}
		}
		else
		{
			Monster::checkHurtTarget(ent, var2);
		}
	}
}