#pragma once

#include "world/entity/Monster.hpp"

class Spider : public Monster
{
public:
	Spider(Level* pLevel);

	std::string getAmbientSound() const override { return "mob.spider"; };
	std::string getDeathSound() const override { return "mob.spiderdeath"; }
	std::string getHurtSound() const override { return "mob.spider"; }
	int getDeathLoot() const override { return ITEM_STRING; }
	
	float getRideHeight() 
	{
		return m_bbHeight * 0.75f - 0.5f;
	}

	Entity* findAttackTarget() override
	{
		if (getBrightness(1.0f) < 0.5f)
			return m_pLevel->getNearestPlayer(m_pos, 16.0f);
		
		return nullptr;
	}

	void checkHurtTarget(Entity* ent, float var2) override 
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
				if (m_onGround)
				{
					float var4 = ent->m_pos.x - m_pos.x;
					float var6 = ent->m_pos.z - m_pos.z;
					float var8 = Mth::sqrt(var4 * var4 + var6 * var6);
					m_pos.x = var4 / var8 * .5f * .800000011920929f + m_pos.x * .20000000298023224f;
					m_pos.z = var6 / var8 * 0.5f * 0.800000011920929f + m_pos.z * 0.20000000298023224f;
					m_pos.y = 0.4000000059604645f;
				}
			}
			else 
			{
				Monster::checkHurtTarget(ent, var2);
			}
		}
	}
	
	bool onLadder() const override { return m_bHorizontalCollision; }
};