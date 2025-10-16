/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ItemEntity.hpp"
#include "world/level/Level.hpp"
#include "nbt/CompoundTag.hpp"

void ItemEntity::_init(ItemInstance* itemInstance)
{
	m_pDescriptor = &EntityTypeDescriptor::item;
	field_C8 = RENDER_ITEM;
	m_age = 0;
	m_throwTime = 0;
	m_tickCount = 0;
	m_health = 5;
	m_bMakeStepSound = false;

	// @NOTE: not setting render type
	m_bobOffs = 2 * float(M_PI) * Mth::random();
	setSize(0.25f, 0.25f);
	m_heightOffset = m_bbHeight * 0.5f;
	m_pItemInstance = itemInstance;
}

void ItemEntity::_init(ItemInstance* itemInstance, const Vec3& pos)
{
	_init(itemInstance);
	setPos(pos);

	m_rot.x = 360.0f * Mth::random();

	m_vel.y = 0.2f;
	m_vel.x = Mth::random() * 0.2f - 0.1f;
	m_vel.z = Mth::random() * 0.2f - 0.1f;
}

ItemEntity::~ItemEntity()
{
	SAFE_DELETE(m_pItemInstance);
}

void ItemEntity::burn(int damage)
{
	hurt(nullptr, damage);
}

bool ItemEntity::hurt(Entity* pCulprit, int damage)
{
	markHurt();
	m_health -= damage;
	if (m_health < 0)
		remove();

	return false;
}

bool ItemEntity::isInWater()
{
	return m_pLevel->checkAndHandleWater(m_hitbox, Material::water, this);
}

void ItemEntity::playerTouch(Player* player)
{
	if (m_pLevel->m_bIsClientSide)
		return;

	// Here, this would give the item to the player, and remove the item entity.
	if (m_throwTime != 0 || !player->isAlive())
		return;

	Inventory* pInventory = player->m_pInventory;

	if (!pInventory->addItem(*m_pItemInstance))
		return;

	m_pLevel->playSound(this, "random.pop", 0.3f,
		((sharedRandom.nextFloat() - sharedRandom.nextFloat()) * 0.7f + 1.0f) * 2.0f);

	player->take(this, m_pItemInstance->m_count);

	// On 0.2.1, this gets removed regardless. What about stacks??
	if (m_pItemInstance->m_count <= 0)
		remove();
}

void ItemEntity::tick()
{
	Entity::tick();

	if (m_throwTime > 0)
		m_throwTime--;

	m_oPos = m_pos;
	m_vel.y -= 0.04f;

	if (m_pLevel->getMaterial(m_pos) == Material::lava)
	{
		// give it a small bounce upwards
		m_vel.y = 0.2f;
		m_vel.x = 0.2f * (sharedRandom.nextFloat() - sharedRandom.nextFloat());
		m_vel.z = 0.2f * (sharedRandom.nextFloat() - sharedRandom.nextFloat());
	}

	checkInTile(m_pos);

	move(m_vel);

	float dragFactor = 0.98f;

	if (m_bOnGround)
	{
		dragFactor = 0.588f;
		TileID tile = m_pLevel->getTile(TilePos(Mth::floor(m_pos.x), Mth::floor(m_hitbox.min.y) - 1, Mth::floor(m_pos.z)));
		if (tile > 0)
			dragFactor = Tile::tiles[tile]->m_friction * 0.98f;
	}

	m_vel.x *= dragFactor;
	m_vel.z *= dragFactor;
	m_vel.y *= 0.98f;

	if (m_bOnGround)
		m_vel.y *= -0.5f;

	m_tickCount++;
	m_age++;

	// despawn after 5 minutes
	if (m_age >= 6000)
		remove();
}

void ItemEntity::addAdditionalSaveData(CompoundTag& tag) const
{
	tag.putInt16("Health", m_health);
	tag.putInt16("Age", m_age);
	CompoundTag* itemTag = new CompoundTag();
	m_pItemInstance->save(*itemTag);
	tag.putCompound("Item", itemTag);
}

void ItemEntity::readAdditionalSaveData(const CompoundTag& tag)
{
	m_health = tag.getInt16("Health") & 255;
	m_age = tag.getInt16("Age");

	const CompoundTag* itemTag = tag.getCompound("Item");
	if (!itemTag)
	{
		remove();
		return;
	}
	m_pItemInstance = new ItemInstance();
	m_pItemInstance->load(*itemTag);
}

void ItemEntity::checkInTile(const Vec3& pos)
{
	TilePos flPos = pos;

	if (!Tile::solid[m_pLevel->getTile(pos)])
		return;
	
	bool solidXN = Tile::solid[m_pLevel->getTile(flPos.west())];
	bool solidXP = Tile::solid[m_pLevel->getTile(flPos.east())];
	bool solidYN = Tile::solid[m_pLevel->getTile(flPos.below())];
	bool solidYP = Tile::solid[m_pLevel->getTile(flPos.above())];
	bool solidZN = Tile::solid[m_pLevel->getTile(flPos.north())];
	bool solidZP = Tile::solid[m_pLevel->getTile(flPos.south())];

	float mindist = 9999.0f;
	int mindir = -1;

	Vec3 diff = pos - flPos;
	if (!solidXN && diff.x        < mindist) mindist = diff.x,        mindir = 0;
	if (!solidXP && 1.0f - diff.x < mindist) mindist = 1.0f - diff.x, mindir = 1;
	if (!solidYN && diff.y < mindist) mindist = diff.y,        mindir = 2;
	if (!solidYP && 1.0f - diff.y < mindist) mindist = 1.0f - diff.y, mindir = 3;
	if (!solidZN && diff.z < mindist) mindist = diff.z,        mindir = 4;
	if (!solidZP && 1.0f - diff.z < mindist) mindist = 1.0f - diff.z, mindir = 5;

	// the -1 case will be handled accordingly
	float force = 0.1f + 0.2f * sharedRandom.nextFloat();
	switch (mindir)
	{
		case 0: m_vel.x = -force; break;
		case 1: m_vel.x =  force; break;
		case 2: m_vel.y = -force; break;
		case 3: m_vel.y =  force; break;
		case 4: m_vel.z = -force; break;
		case 5: m_vel.z =  force; break;
	}
}
