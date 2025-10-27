/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Player.hpp"
#include "world/level/Level.hpp"
#include "nbt/CompoundTag.hpp"

void Player::_init()
{
	// I just guessed, it's 5/5 fields
	m_score = 0;
	m_oBob = 0.0f;
	m_bob = 0.0f;
	m_dimension = 0;
	m_destroyingBlock = false;
}

Player::Player(Level* pLevel, GameType playerGameType) : Mob(pLevel)
{
	_init();
	m_pDescriptor = &EntityTypeDescriptor::player;
	m_pInventory = nullptr;
	field_B94 = 0;
	m_name = "";
	m_bHasRespawnPos = false;

	field_C8 = RENDER_HUMANOID;

	setPlayerGameType(playerGameType);

	m_pInventory = new Inventory(this);

	setDefaultHeadHeight();

	Vec3 pos = m_pLevel->getSharedSpawnPos();

	moveTo(Vec3(pos.x + 0.5f, pos.y + 1.0f, pos.z + 0.5f));

	m_health = getMaxHealth();

	m_class = "humanoid";
	m_texture = "mob/char.png";

	m_flameTime = 20;
	m_rotOffs = 180.0f;
}

Player::~Player()
{
	delete m_pInventory;
}

void Player::reallyDrop(ItemEntity* pEnt)
{
	m_pLevel->addEntity(pEnt);
}

void Player::reset()
{
	Mob::reset();
	_init();
}

void Player::remove()
{
	m_bIsInvisible = true;
	Mob::remove();
}

bool Player::hurt(Entity* pEnt, int damage)
{
	if (isCreative())
		return false;
    
    m_noActionTime = 0;
    if (m_health <= 0)
    {
        return false;
    }
    
	if (pEnt != nullptr)
	{
		EntityTypeDescriptor entDesc = pEnt->getDescriptor();

		if (entDesc.hasCategory(EntityCategories::MONSTER) || entDesc.hasCategory(EntityCategories::ABSTRACT_ARROW))
		{
			switch (m_pLevel->m_difficulty)
			{
				case 0:
					damage = 0;
					break;
				case 1:
					damage = damage / 3 + 1;
					break;
				case 2:
					// Don't modify damage
					break;
				case 3:
					damage = damage * 3 / 2;
					break;
				default:
					assert(!"Unknown difficulty value");
					break;
			}
		}
	}

    return damage == 0 ? false : Mob::hurt(pEnt, damage);
}

void Player::awardKillScore(Entity* pKilled, int score)
{
	m_score += score;
}

void Player::resetPos(bool respawn)
{
	setDefaultHeadHeight();
	setSize(0.6f, 1.8f);

	Entity::resetPos();
	m_bIsInvisible = false;
	if (respawn)
	{
		m_deathTime = 0;
		m_health = getMaxHealth();
	}
}

void Player::die(Entity* pCulprit)
{
	Mob::die(pCulprit);
	setSize(0.2f, 0.2f);
	setPos(m_pos); // update hitbox
	m_vel.y = 0.1f;

	if (!m_pLevel->m_bIsClientSide)
	{
		if (m_name == "Notch")
			drop(ItemInstance(Item::apple), true);
	}
	m_pInventory->dropAll(m_pLevel->m_bIsClientSide);

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
    if (m_pLevel->m_difficulty == 0 &&
        m_health < 20 &&
        m_tickCount % 20 * 12 == 0)
    {
       heal(1);
    }

#ifdef ENH_GUI_ITEM_POP
    m_pInventory->tick();
#endif
	m_oBob = m_bob;
    //Mob::aiStep(); // called in Java, calling here results in 2x speed player movement
	float velLen = Mth::sqrt(m_vel.x * m_vel.x + m_vel.z * m_vel.z);
	float velYAtan = Mth::atan(m_vel.y * -0.2f), x1 = 0.0f;

	if (velLen > 0.1f)
		velLen = 0.1f;

	if (!m_bOnGround)
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

	m_bob += (velLen - m_bob) * 0.4f;
	m_tilt += (x1 - m_tilt) * 0.8f;

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

	// only needed for non-local players for some reason
	updateAttackAnim();
}

ItemInstance* Player::getCarriedItem()
{
	// This only gets the first row slot
	/*ItemInstance* item = m_pInventory->getItem(m_pInventory->m_selectedHotbarSlot);
  
	if (ItemInstance::isNull(item))
		return nullptr;

	return item;*/

	return m_pInventory->getSelected();
}

void Player::updateAi()
{
}

void Player::addAdditionalSaveData(CompoundTag& tag) const
{
	Mob::addAdditionalSaveData(tag);

	ListTag* inventoryTag = new ListTag();
	m_pInventory->save(*inventoryTag);
	tag.put("Inventory", inventoryTag);

	tag.putInt32("playerGameType", getPlayerGameType());
	tag.putInt32("Dimension", m_dimension);

	// Why would we save the player's sleep state? If they leave the game, just wake them up.
	/*tag.putBoolean("Sleeping", m_bSleeping);
	tag.putShort("SleepTimer", m_sleepTimer);
	if (m_bSleeping)
	{
		setBedSleepPos(m_pos);
		wake(true, true, false);
	}*/

	if (m_bHasRespawnPos)
	{
		tag.putInt32("SpawnX", m_respawnPos.x);
		tag.putInt32("SpawnY", m_respawnPos.y);
		tag.putInt32("SpawnZ", m_respawnPos.z);
	}
}

void Player::readAdditionalSaveData(const CompoundTag& tag)
{
	Mob::readAdditionalSaveData(tag);

	// Needs to load before Inventory, since Inventory won't load if Player is in creative mode
	if (tag.contains("playerGameType"))
	{
		setPlayerGameType((GameType)tag.getInt32("playerGameType"));
	}

	if (tag.contains("Inventory"))
		m_pInventory->load(*tag.getList("Inventory"));

	m_dimension = tag.getInt32("Dimension");
	//m_sleepTimer = tag.getInt32("SleepTimer");

	if (tag.contains("SpawnX") && tag.contains("SpawnY") && tag.contains("SpawnZ")) {
		setRespawnPos(TilePos(	static_cast<int>(tag.getInt32("SpawnX")),
								static_cast<int>(tag.getInt32("SpawnY")),
								static_cast<int>(tag.getInt32("SpawnZ"))));
	}
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

void Player::useItem(ItemInstance& item) const
{
	if (!isCreative())
		item.remove(1);
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

int Player::getInventorySlot(int x) const
{
	return 0;
}

void Player::prepareCustomTextures()
{

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
		m_bHasRespawnPos = false;
		return;
	}*/

	m_bHasRespawnPos = true;
	m_respawnPos = pos;
}

void Player::drop()
{
	// From b1.2_02, doesn't exist in PE
	// Isn't called anywhere, but is overriden in MultiplayerLocalPlayer with a PlayerActionPacket
	/*ItemInstance* item = getSelectedItem();
	if (!item)
		return;

	drop(m_pInventory->removeItem(*item, 1));*/
}

void Player::drop(const ItemInstance& item, bool randomly)
{
	if (item.isNull())
		return;

	ItemEntity* pItemEntity = new ItemEntity(m_pLevel, Vec3(m_pos.x, m_pos.y - 0.3f + getHeadHeight(), m_pos.z), item.copy());
	pItemEntity->m_throwTime = 40;

	if (randomly)
	{
		float throwPower = 0.5f * m_random.nextFloat();
		float throwAngle = m_random.nextFloat();

		pItemEntity->m_vel.x = -(throwPower * Mth::sin(2 * float(M_PI) * throwAngle));
		pItemEntity->m_vel.z = (throwPower * Mth::cos(2 * float(M_PI) * throwAngle));
		pItemEntity->m_vel.y = 0.2f;
	}
	else
	{
		pItemEntity->m_vel.x = -(Mth::sin(m_rot.x / 180.0f * float(M_PI)) * Mth::cos(m_rot.y / 180.0f * float(M_PI))) * 0.3f;
		pItemEntity->m_vel.z = (Mth::cos(m_rot.x / 180.0f * float(M_PI)) * Mth::cos(m_rot.y / 180.0f * float(M_PI))) * 0.3f;
		pItemEntity->m_vel.y = 0.1f - Mth::sin(m_rot.y / 180.0f * float(M_PI)) * 0.3f;

		float f1 = m_random.nextFloat();
		float f2 = m_random.nextFloat();

		pItemEntity->m_vel.x += 0.02f * f2 * Mth::cos(2 * float(M_PI) * f1);
		pItemEntity->m_vel.y += 0.1f * (m_random.nextFloat() - m_random.nextFloat());
		pItemEntity->m_vel.z += 0.02f * f2 * Mth::sin(2 * float(M_PI) * f1);
	}

	reallyDrop(pItemEntity);
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

void Player::removeSelectedItem()
{
	m_pInventory->setSelectedItem(nullptr);
}
