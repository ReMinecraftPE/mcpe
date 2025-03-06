/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Player.hpp"
#include "world/level/Level.hpp"

Player::Player(Level* pLevel, GameType playerGameType) : Mob(pLevel)
{
	m_pDescriptor = &EntityTypeDescriptor::player;
	m_pInventory = nullptr;
	field_B94 = 0;
	m_score = 0;
	field_B9C = 0.0f;
	field_BA0 = 0.0f;
	m_name = "";
	field_BC4 = 0;
	m_bHaveRespawnPos = false;
	m_destroyingBlock = false;

	field_C8 = RENDER_HUMANOID;

	setPlayerGameType(playerGameType);

	m_pInventory = new Inventory(this);

	setDefaultHeadHeight();

	Vec3 pos = m_pLevel->getSharedSpawnPos();

	moveTo(Vec3(pos.x + 0.5f, pos.y + 1.0f, pos.z + 0.5f));

	m_health = 20;

	m_class = "humanoid";
	m_texture = "mob/char.png";

	m_flameTime = 20;
	m_rotOffs = 180.0f;
}

Player::~Player()
{
	delete m_pInventory;
}

void Player::reset()
{
	Mob::reset();

	// TODO what fields to reset???
}

bool Player::hurt(Entity* pEnt, int damage)
{
	if (isCreative())
		return false;

	return Mob::hurt(pEnt, damage);
}

void Player::awardKillScore(Entity* pKilled, int score)
{
	m_score += score;
}

void Player::resetPos()
{
	setDefaultHeadHeight();
	setSize(0.6f, 1.8f);

	Entity::resetPos();

	m_health = 20;
	field_110 = 0;
}

void Player::die(Entity* pCulprit)
{
	Mob::die(pCulprit);
	setSize(0.2f, 0.2f);
	setPos(m_pos); // update hitbox
	m_vel.y = 0.1f;

	if (m_name == "Notch")
		drop(new ItemInstance(Item::apple), true);
	m_pInventory->dropAll();

	if (pCulprit)
	{
		m_vel.x = -0.1f * Mth::cos(float((m_hurtDir + m_rot.x) * M_PI / 180.0));
		m_vel.z = -0.1f * Mth::cos(float((m_hurtDir + m_rot.x) * M_PI / 180.0));
	}
	else
	{
		m_vel.x = 0;
		m_vel.z = 0;
	}
}

void Player::aiStep()
{
	field_B9C = field_BA0;
	float velLen = Mth::sqrt(m_vel.x * m_vel.x + m_vel.z * m_vel.z);
	float velYAtan = Mth::atan(m_vel.y * -0.2f), x1 = 0.0f;

	if (velLen > 0.1f)
		velLen = 0.1f;

	if (!m_onGround)
	{
		if (m_health > 0)
		{
			velLen = 0;
			x1 = velYAtan * 15.0f;
		}
		else
		{
			velLen = x1 = 0.0f;
		}
	}

	if (m_health <= 0)
		velLen = 0.0f;

	field_BA0 += (velLen - field_BA0) * 0.4f;
	field_11C += (x1 - field_11C) * 0.8f;

	if (m_health <= 0)
		return;

	AABB scanAABB = m_hitbox;
	scanAABB.grow(1, 1, 1);

	EntityVector ents = m_pLevel->getEntities(this, scanAABB);

	for (EntityVector::iterator it = ents.begin(); it != ents.end(); it++)
	{
		Entity* pEnt = *it;
		if (pEnt->m_bRemoved)
			continue;

		touch(pEnt);
	}
}

void Player::updateAi()
{
	if (m_bSwinging)
	{
		m_swingTime++;
		if (m_swingTime >= 8)
		{
			m_swingTime = 0;
			m_bSwinging = false;
		}
	}
	else
	{
		m_swingTime = 0;
	}

	m_attackAnim = m_swingTime / 8.0f;
}

void Player::animateRespawn()
{
	
}

int Player::addResource(int x)
{
	return 0;
}

void Player::animateRespawn(Player*, Level*)
{

}

void Player::attack(Entity* pEnt)
{
	int atkDmg = m_pInventory->getAttackDamage(pEnt);
	if (atkDmg > 0)
		pEnt->hurt(this, atkDmg);
}

bool Player::canDestroy(const Tile* pTile) const
{
	return true;
}

void Player::closeContainer()
{

}

void Player::displayClientMessage(const std::string& msg)
{

}

void Player::drop(const ItemInstance* pItemInstance, bool b)
{
	if (pItemInstance->isNull())
		return;

	ItemEntity* pItemEntity = new ItemEntity(m_pLevel, Vec3(m_pos.x, m_pos.y - 0.3f + getHeadHeight(), m_pos.z), pItemInstance);
	pItemEntity->field_E4 = 40;

	if (b)
	{
		float throwPower = 0.5f * m_random.nextFloat();
		float throwAngle = m_random.nextFloat();

		pItemEntity->m_vel.x = -(throwPower * Mth::sin(2 * float(M_PI) * throwAngle));
		pItemEntity->m_vel.z =  (throwPower * Mth::cos(2 * float(M_PI) * throwAngle));
		pItemEntity->m_vel.y = 0.2f;
	}
	else
	{
		pItemEntity->m_vel.x = -(Mth::sin(m_rot.x / 180.0f * float(M_PI)) * Mth::cos(m_rot.y / 180.0f * float(M_PI))) * 0.3f;
		pItemEntity->m_vel.z =  (Mth::cos(m_rot.x / 180.0f * float(M_PI)) * Mth::cos(m_rot.y / 180.0f * float(M_PI))) * 0.3f;
		pItemEntity->m_vel.y = 0.1f - Mth::sin(m_rot.y / 180.0f * float(M_PI)) * 0.3f;

		float f1 = m_random.nextFloat();
		float f2 = m_random.nextFloat();

		pItemEntity->m_vel.x += 0.02f * f2 * Mth::cos(2 * float(M_PI) * f1);
		pItemEntity->m_vel.y += 0.1f * (m_random.nextFloat() - m_random.nextFloat());
		pItemEntity->m_vel.z += 0.02f * f2 * Mth::sin(2 * float(M_PI) * f1);
	}

	reallyDrop(pItemEntity);
}

void Player::drop()
{

}

int Player::getInventorySlot(int x) const
{
	return 0;
}

void Player::prepareCustomTextures()
{

}

void Player::reallyDrop(ItemEntity* pEnt)
{
	m_pLevel->addEntity(pEnt);
}

void Player::respawn()
{

}

void Player::rideTick()
{

}

void Player::setDefaultHeadHeight()
{
	m_heightOffset = 1.62f;
}

void Player::setRespawnPos(const TilePos& pos)
{
	/*if (!pos)
	{
		m_bHaveRespawnPos = false;
		return;
	}*/

	m_bHaveRespawnPos = true;
	m_respawnPos = pos;
}

void Player::startCrafting(const TilePos& pos)
{

}

void Player::startStonecutting(const TilePos& pos)
{

}

void Player::startDestroying()
{
	m_destroyingBlock = true;
}

void Player::stopDestroying()
{
	m_destroyingBlock = false;
}

void Player::take(Entity* pEnt, int x)
{

}

void Player::touch(Entity* pEnt)
{
	pEnt->playerTouch(this);
}

void Player::interact(Entity* pEnt)
{
	pEnt->interact(this);
}

ItemInstance* Player::getSelectedItem() const
{
	return m_pInventory->getSelected();
}