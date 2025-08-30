#include "world/entity/Monster.hpp"

Monster::Monster(Level* pLevel) : PathfinderMob(pLevel)
{
	m_health = 20;
	m_attackDamage = 2;
}

void Monster::aiStep()
{
	float brightness = getBrightness(1.0f);

	if (brightness > 0.5f) 
	{
		m_noActionTime += 2;
	}

	PathfinderMob::aiStep();
}

void Monster::tick()
{
	PathfinderMob::tick();

	// Remove any monsters when in peaceful mode
	if (m_pLevel->m_difficulty == 0) 
	{
		remove();
	}
}

Entity* Monster::findAttackTarget()
{
	Player* player = m_pLevel->getNearestAttackablePlayer(*this, 16.0f);

	if (player && canSee(player)) 
	{
		return player;
	}

	return nullptr;
}

bool Monster::hurt(Entity* pCulprit, int damage)
{
	if (PathfinderMob::hurt(pCulprit, damage)) 
	{
		if (pCulprit != this) 
		{
			m_pAttackTarget = pCulprit;
		}
		return true;
	}

	return false;
}

void Monster::checkHurtTarget(Entity* pEnt, float f)
{
	if (m_attackTime <= 0 && f < 2.0f && pEnt->m_hitbox.max.y > m_hitbox.min.y && pEnt->m_hitbox.min.y < m_hitbox.max.y) 
	{
		m_attackTime = 20;
		pEnt->hurt(this, m_attackDamage);
	}
}

float Monster::getWalkTargetValue(const TilePos& pos) const
{
	float brightness = m_pLevel->getBrightness(pos);

	return 0.5f - brightness;
}

bool Monster::canSpawn()
{
	TilePos pos(m_pos.x, m_hitbox.min.y, m_pos.z);

	if (m_pLevel->getBrightness(LightLayer::Sky, pos) > m_random.nextInt(30)) 
	{
		return false;
	}
	else if (m_pLevel->getBrightness(pos) <= m_random.nextInt(8)) 
	{
		return PathfinderMob::canSpawn();
	}

	return false;
}

void Monster::addAdditionalSaveData(CompoundTag& tag) const
{
	Mob::addAdditionalSaveData(tag);
}

void Monster::readAdditionalSaveData(const CompoundTag& tag)
{
	Mob::readAdditionalSaveData(tag);
}