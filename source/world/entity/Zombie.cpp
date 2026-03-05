#include "Zombie.hpp"
#include "world/level/TileSource.hpp"

Zombie::Zombie(TileSource& source) : Monster(source)
{
	m_pDescriptor = &EntityTypeDescriptor::zombie;
	m_renderType = RENDER_ZOMBIE;
	m_texture = "mob/zombie.png";
	m_runSpeed = 0.5f;
	m_attackDamage = 5;
}

void Zombie::aiStep()
{
	if (m_tileSource->getDimensionConst().isDay())
	{
		float var1 = getBrightness(1.0f);
		if (var1 > 0.5f
			&& m_tileSource->canSeeSky(this->m_pos)
			&& m_random.nextFloat() * 30.0f < (var1 - 0.4f) * 2.0f)
		{
			this->m_fireTicks = 300;
		}
	}

	Monster::aiStep();
}
