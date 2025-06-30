#include "Zombie.hpp"

Zombie::Zombie(Level* pLevel) : Monster(pLevel)
{
	m_pDescriptor = &EntityTypeDescriptor::zombie;
	field_C8 = RENDER_ZOMBIE;
	m_texture = "mob/zombie.png";
	m_runSpeed = 0.5f;
	m_attackDamage = 5;
}

void Zombie::aiStep()
{
	if (m_pLevel->isDay())
	{
		float var1 = getBrightness(1.0f);
		if (var1 > 0.5f
			&& m_pLevel->canSeeSky(this->m_pos)
			&& m_random.nextFloat() * 30.0f < (var1 - 0.4f) * 2.0f)
		{
			this->m_fireTicks = 300;
		}
	}

	Monster::aiStep();
}