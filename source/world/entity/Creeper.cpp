#include "world/entity/Creeper.hpp"

Creeper::Creeper(Level* pLevel) : Monster(pLevel)
{
	m_pDescriptor = &EntityTypeDescriptor::creeper;
	field_C8 = RENDER_CREEPER;
	m_texture = "mob/creeper.png";
	m_swell = 0;
	m_oldSwell = 0;

	_defineEntityData();
}

void Creeper::_defineEntityData()
{
	m_entityData.define<int>(DATA_SWELL_DIR, -1);
}

void Creeper::tick()
{
	m_oldSwell = m_swell;
	if (m_pLevel->m_bIsClientSide)
	{
		int swellDir = getSwellDir();
		if (swellDir > 0 && m_swell == 0)
		{
			m_pLevel->playSound(this, "random.fuse", 1.0f, 0.5f);
		}

		m_swell += swellDir;

		if (m_swell < 0)
		{
			m_swell = 0;
		}
		else if (m_swell > MAX_SWELL)
		{
			m_swell = MAX_SWELL;
		}
	}

	Monster::tick();
}

void Creeper::die(Entity* pCulprit)
{
	Monster::die(pCulprit);

	if (pCulprit && pCulprit->getDescriptor().isType(EntityType::SKELETON))
	{
		spawnAtLocation(Item::record_01->m_itemID + m_random.nextInt(2), 1);
	}
}

void Creeper::checkHurtTarget(Entity* pEnt, float f)
{
	int swellDir = getSwellDir();
	if ((swellDir <= 0 && f < 3.0f) || (swellDir > 0 && f < 7.0f))
	{
		if (m_swell == 0)
		{
			m_pLevel->playSound(this, "random.fuse", 1.0f, 0.5f);
		}

		setSwellDir(1);
		m_swell++;
		if (m_swell >= MAX_SWELL)
		{
			m_pLevel->explode(this, m_pos, 3.0f);
			remove();
		}

		m_bHoldGround = true;
	}
	else
	{
		setSwellDir(-1);
		m_swell--;
		if (m_swell < 0) {
			m_swell = 0;
		}
	}
}

float Creeper::getSwelling(float f) const
{
	return (m_oldSwell + (m_swell - m_oldSwell) * f) / 28.0f;
}