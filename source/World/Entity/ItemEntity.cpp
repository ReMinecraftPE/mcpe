/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ItemEntity.hpp"
#include "Level.hpp"

ItemEntity::ItemEntity(Level* level) : Entity(level)
#ifndef ORIGINAL_CODE
, m__itemInstance(0, 0, 0)
#endif
{
	// @NOTE: not setting render type
	field_E8 = 2 * float(M_PI) * Mth::random();
	setSize(0.25f, 0.25f);
	field_84 = field_8C * 0.5f;
}

ItemEntity::ItemEntity(Level* level, float x, float y, float z, ItemInstance* itemInstance) :
	Entity(level)
#ifndef ORIGINAL_CODE
	, m__itemInstance(0, 0, 0)
#endif
{
	field_C8 = RENDER_ITEM;
	field_E8 = 2 * float(M_PI) * Mth::random();
	setSize(0.25f, 0.25f);
	field_84 = field_8C * 0.5f;
	setPos(x, y, z);

	// @BUG: Keeping a pointer to the original ItemInstance may end up storing an invalid one
#ifdef ORIGINAL_CODE
	m_pItemInstance = itemInstance;
#else
	// copy it
	m__itemInstance = *itemInstance;
#endif

	m_yaw = 360.0f * Mth::random();

	m_vel.y = 0.2f;
	m_vel.x = Mth::random() * 0.2f - 0.1f;
	m_vel.z = Mth::random() * 0.2f - 0.1f;
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
	// Here, this would give the item to the player, and remove the item entity.
#ifdef TEST_DROPPED_ITEMS
	if (field_E4 != 0)
		return;

	// No inventory to add an item to. Just set the amount to 0
	m_pItemInstance->m_amount = 0;

	// No "random.pop" sound.

	if (m_pItemInstance->m_amount <= 0)
		remove();
#endif
}

void ItemEntity::tick()
{
	Entity::tick();

	if (field_E4 > 0)
		field_E4--;

	field_3C = m_pos;
	m_vel.y -= 0.04f;

	if (m_pLevel->getMaterial(Mth::floor(m_pos.x), Mth::floor(m_pos.y), Mth::floor(m_pos.z)) == Material::lava)
	{
		// give it a small bounce upwards
		m_vel.y = 0.2f;
		m_vel.x = 0.2f * (sharedRandom.nextFloat() - sharedRandom.nextFloat());
		m_vel.z = 0.2f * (sharedRandom.nextFloat() - sharedRandom.nextFloat());
	}

	checkInTile(m_pos.x, m_pos.y, m_pos.z);

	move(m_vel.x, m_vel.y, m_vel.z);

	float dragFactor = 0.98f;

	if (field_7C)
	{
		dragFactor = 0.588f;
		TileID tile = m_pLevel->getTile(Mth::floor(m_pos.x), Mth::floor(m_hitbox.min.y) - 1, Mth::floor(m_pos.z));
		if (tile > 0)
			dragFactor = Tile::tiles[tile]->field_30 * 0.98f;
	}

	m_vel.x *= dragFactor;
	m_vel.z *= dragFactor;
	m_vel.y *= 0.98f;

	if (field_7C)
		m_vel.y *= -0.5f;

	field_EC++;
	field_E0++;

	//despawn after 5 minutes
	if (field_E0 >= 6000)
		remove();
}

void ItemEntity::checkInTile(float x, float y, float z)
{
	int xfl = Mth::floor(x);
	int yfl = Mth::floor(y);
	int zfl = Mth::floor(z);

	if (!Tile::solid[m_pLevel->getTile(xfl, yfl, zfl)])
		return;
	
	bool solidXN = Tile::solid[m_pLevel->getTile(xfl - 1, yfl, zfl)];
	bool solidXP = Tile::solid[m_pLevel->getTile(xfl + 1, yfl, zfl)];
	bool solidYN = Tile::solid[m_pLevel->getTile(xfl, yfl - 1, zfl)];
	bool solidYP = Tile::solid[m_pLevel->getTile(xfl, yfl + 1, zfl)];
	bool solidZN = Tile::solid[m_pLevel->getTile(xfl, yfl, zfl - 1)];
	bool solidZP = Tile::solid[m_pLevel->getTile(xfl, yfl, zfl + 1)];

	float mindist = 9999.0f;
	int mindir = -1;

	float xdiff = x - float(xfl);
	float ydiff = y - float(yfl);
	float zdiff = z - float(zfl);
	if (!solidXN && xdiff        < mindist) mindist = xdiff,        mindir = 0;
	if (!solidXP && 1.0f - xdiff < mindist) mindist = 1.0f - xdiff, mindir = 1;
	if (!solidYN && ydiff        < mindist) mindist = ydiff,        mindir = 2;
	if (!solidYP && 1.0f - ydiff < mindist) mindist = 1.0f - ydiff, mindir = 3;
	if (!solidZN && zdiff        < mindist) mindist = zdiff,        mindir = 4;
	if (!solidZP && 1.0f - zdiff < mindist) mindist = 1.0f - zdiff, mindir = 5;

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
