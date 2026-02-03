/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Player.hpp"
#include "world/level/Level.hpp"
#include "world/tile/BedTile.hpp"
#include "nbt/CompoundTag.hpp"

void Player::_init()
{
	// I just guessed, it's 5/5 fields
	m_score = 0;
	m_oBob = 0.0f;
	m_bob = 0.0f;
	m_dimension = 0;
	m_destroyingBlock = false;
	m_bSleeping = false;
	m_sleepTimer = 0;
}

Player::Player(Level* pLevel, GameType playerGameType) : Mob(pLevel)
{
	_init();
	m_pDescriptor = &EntityTypeDescriptor::player;
	m_pInventory = nullptr;
	field_B94 = 0;
	m_name = "";
	m_bHasRespawnPos = false;

	m_renderType = RENDER_HUMANOID;

	setPlayerGameType(playerGameType);

	m_pInventory = new Inventory(this);

	m_pContainerMenu = m_pInventoryMenu = new InventoryMenu(m_pInventory);

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
	delete m_pInventoryMenu;
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
	m_pInventoryMenu->removed(this);
	if (m_pContainerMenu)
		m_pContainerMenu->removed(this);
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
			drop(ItemStack(Item::apple), true);
	}
#if NETWORK_PROTOCOL_VERSION <= 3
	m_pInventory->dropAll(m_pLevel->m_bIsClientSide);
#endif

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

	// Handle sleeping
	if (m_bSleeping) {
		++m_sleepTimer;
		if (m_sleepTimer > 100) {
			m_sleepTimer = 100;
		}

		if (!checkBed()) {
			stopSleepInBed(true, true, false);
		} 
		else if (!m_pLevel->m_bIsClientSide && m_pLevel->isDay()) {
			stopSleepInBed(false, true, false);
		}
		
		// Check if all players are sleeping to skip time to morning
		if (m_sleepTimer >= 100) {
			m_pLevel->updateSleeping();
		}

		return;
	} else if (m_sleepTimer > 0) {
		++m_sleepTimer;
		if (m_sleepTimer >= 110) {
			m_sleepTimer = 0;
		}
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

void Player::tick()
{
	Mob::tick();

	if (!m_pLevel->m_bIsClientSide)
	{
		if (m_pContainerMenu && !m_pContainerMenu->stillValid(this))
			closeContainer();
	}
}

const ItemStack& Player::getCarriedItem() const
{
	// This only gets the first row slot
	/*ItemStack* item = m_pInventory->getItem(m_pInventory->m_selectedSlot);
  
	if (ItemStack::isNull(item))
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

void Player::useItem(ItemStack& item) const
{
	if (!isCreative())
		item.shrink(1);
}

bool Player::canDestroy(const Tile* pTile) const
{
	// If the tile's material does not need tool check then allow destroy regardless of equipped item
	if (pTile->m_pMaterial->isMineable())
		return true;

	ItemStack& item = getSelectedItem();
	if (!item.isEmpty())
		return item.canDestroySpecial(pTile);

	return false;
}

void Player::closeContainer()
{

}

void Player::displayClientMessage(const std::string& msg)
{

}

float Player::getDestroySpeed(const Tile* tile) const
{
	float speed = 1.0f;
	
	ItemStack& item = getSelectedItem();
	if (!item.isEmpty())
	{
		// Original multiplies but there's no need since you're just multiplying by 1 on the first check.
		speed = item.getDestroySpeed(tile);
	}

	// Speed penalty for being underwater
	if (isUnderLiquid(Material::water))
	{
		speed /= 5.0f;
	}

	// Speed penalty for jumping/falling
	if (!m_bOnGround)
	{
		speed /= 5.0f;
	}

	return speed;
}

int Player::getInventorySlot(int x) const
{
	return 0;
}

Dimension* Player::getDimension() const
{
	return m_pLevel->getDimension(getDimensionId());
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

void Player::updateSleepingPos(Facing::Name direction)
{
	m_sleepingPos.x = 0.0f;
	m_sleepingPos.y = 0.0f;
	m_sleepingPos.z = 0.0f;
	switch (direction) {
		case Facing::SOUTH:  // +Z facing
			m_sleepingPos.z = -1.8f;
			break;
		case Facing::WEST:  // -X facing
			m_sleepingPos.x = 1.8f;
			break;
		case Facing::NORTH:  // -Z facing
			m_sleepingPos.z = 1.8f;
			break;
		default:  // +X facing
			m_sleepingPos.x = -1.8f;
			break;
	}
}

Player::BedSleepingProblem Player::startSleepInBed(const TilePos& pos)
{
	if (isSleeping() || !isAlive())
		return BED_SLEEPING_OTHER_PROBLEM;

	if (m_pLevel->m_pDimension->m_bFoggy)
		return BED_SLEEPING_NOT_POSSIBLE_HERE;

	if (m_pLevel->isDay())
		return BED_SLEEPING_NOT_POSSIBLE_NOW;

	if (Mth::abs(m_pos.x - pos.x) > 3.0f || Mth::abs(m_pos.y - pos.y) > 2.0f || Mth::abs(m_pos.z - pos.z) > 3.0f)
		return BED_SLEEPING_TOO_FAR_AWAY;

	// Check if bed is obstructed (has blocks on top)
	TilePos aboveBed = pos.above();
	if (!m_pLevel->isEmptyTile(aboveBed))
		return BED_SLEEPING_OTHER_PROBLEM;

	setSize(0.2f, 0.2f);
	m_heightOffset = 0.2f;
	
	if (!m_pLevel->isEmptyTile(pos)) {
		TileData data = m_pLevel->getData(pos);
		Facing::Name dir = BedTile::getDirectionFromData(data);
		float xOff = 0.5f;
		float zOff = 0.5f;
		switch (dir) {
			case Facing::SOUTH:
				zOff = 0.9f;
				break;
			case Facing::WEST:
				xOff = 0.1f;
				break;
			case Facing::NORTH:
				zOff = 0.1f;
				break;
			case Facing::EAST:
				xOff = 0.9f;
				break;
			default:
				break;
		}
		updateSleepingPos(dir);
		setPos(Vec3(pos.x + xOff, pos.y + 1.1f, pos.z + zOff));
	}
	else {
		setPos(Vec3(pos.x + 0.5f, pos.y + 1.1f, pos.z + 0.5f));
	}

	m_bSleeping = true;
	m_sleepTimer = 0;
	
	setRespawnPos(pos);
	m_vel = Vec3::ZERO;

	m_pLevel->updateSleeping();

	return BED_SLEEPING_OK;
}

void Player::stopSleepInBed(bool resetCounter, bool update, bool setSpawn)
{
	setSize(0.6f, 1.8f);
	setDefaultHeadHeight();
	
	TilePos checkBedPos = m_respawnPos;
	if (m_bHasRespawnPos && m_pLevel->getTile(checkBedPos) == Tile::bed->m_ID) {
		BedTile::setBedOccupied(m_pLevel, checkBedPos, false);
		checkBedPos = BedTile::getRespawnTilePos(m_pLevel, checkBedPos, 0);
		if (checkBedPos == m_respawnPos)
			checkBedPos = checkBedPos.above();

		setPos(Vec3(checkBedPos.x + 0.5f, checkBedPos.y + m_heightOffset + 0.1f, checkBedPos.z + 0.5f));
	}

	m_bSleeping = false;
	
	if (!m_pLevel->m_bIsClientSide && update) {
		m_pLevel->updateSleeping();
	}
	
	if (resetCounter) {
		m_sleepTimer = 0;
	} else {
		m_sleepTimer = 100;
	}
}

float Player::getBedSleepRot() const
{
	if (!m_pLevel || !m_bHasRespawnPos)
		return 0.0f;
	
	if (m_respawnPos.y < 0 || m_respawnPos.y >= 128)
		return 0.0f;

	TileData data = m_pLevel->getData(m_respawnPos);
	Facing::Name dir = BedTile::getDirectionFromData(data);
	
	switch (dir) {
		case Facing::SOUTH:
			return 90.0f;
		case Facing::WEST:
			return 0.0f;
		case Facing::NORTH:
			return 270.0f;
		case Facing::EAST:
			return 180.0f;
		default:
			return 0.0f;
	}
}

bool Player::checkBed() const
{
	return m_pLevel->getTile(m_respawnPos) == Tile::bed->m_ID;
}

/*void Player::drop()
{
	// From b1.2_02, doesn't exist in PE
	// Isn't called anywhere, but is overriden in MultiplayerLocalPlayer with a PlayerActionPacket
	ItemStack* item = getSelectedItem();
	if (!item)
		return;

	drop(m_pInventory->removeItem(*item, 1));
}*/

void Player::drop(const ItemStack& item, bool randomly)
{
	if (item.isEmpty())
		return;

	ItemEntity* pItemEntity = new ItemEntity(m_pLevel, Vec3(m_pos.x, m_pos.y - 0.3f + getHeadHeight(), m_pos.z), item);
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

ItemStack& Player::getSelectedItem() const
{
	return m_pInventory->getSelected();
}

void Player::removeSelectedItem()
{
	m_pInventory->setSelectedItem(ItemStack::EMPTY);
}
