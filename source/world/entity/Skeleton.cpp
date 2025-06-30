#include "Skeleton.hpp"

Skeleton::Skeleton(Level* pLevel) : Monster(pLevel)
{
	m_pDescriptor = &EntityTypeDescriptor::skeleton;
	field_C8 = RENDER_SKELETON;
	m_texture = "mob/skeleton.png";
}

void Skeleton::aiStep()
{
	if (m_pLevel->isDay())
	{
		float brightness = getBrightness(1.0f);
		if (brightness > 0.5f
			&& m_pLevel->canSeeSky(this->m_pos)
			&& m_random.nextFloat() * 30.0f < (brightness - 0.4f) * 2.0f)
		{
			this->m_fireTicks = 300;
		}
	}

	Monster::aiStep();
}

void Skeleton::checkHurtTarget(Entity* ent, float f)
{
	if (f < 10.0f)
	{
		float delta_x = ent->m_pos.x - m_pos.x;
		float delta_z = ent->m_pos.z - m_pos.z;
		if (m_attackTime == 0)
		{
			Arrow* arrow = new Arrow(m_pLevel, this);
			arrow->m_pos.y += 1;
			float var8 = ent->m_pos.y - 0.2f - arrow->m_pos.y;
			float var10 = Mth::sqrt(delta_x * delta_x + delta_z * delta_z) * 0.2f;
			m_pLevel->playSound(this, "random.bow", 1.0f, 1.0f / (m_random.nextFloat() * 0.4f + 0.8f));
			m_pLevel->addEntity(arrow);
			arrow->shoot(delta_x, var8 + var10, delta_z, 0.6f, 12.0f);
			m_attackTime = 30;
		}

		m_rot.y = (Mth::atan2(delta_z, delta_x) * 180.0f / M_PI) - 90.0f;
		m_bHoldGround = true;
	}

}

void Skeleton::dropDeathLoot()
{
	for (int i = 0; i < m_random.nextInt(3); ++i)
		spawnAtLocation(Item::arrow->m_itemID, 1);

	for (int i = 0; i < m_random.nextInt(3); ++i)
		spawnAtLocation(Item::bone->m_itemID, 1);
}

ItemInstance* Skeleton::bow = nullptr;
ItemInstance* Skeleton::getCarriedItem()
{
	if (!bow)
		bow = new ItemInstance(Item::bow, 1);

	return bow;
}
