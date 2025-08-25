/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Entity.hpp"

class ItemEntity : public Entity
{
private:
	void _init(const ItemInstance* itemInstance = nullptr);
	void _init(const ItemInstance* itemInstance, const Vec3& pos);
public:
	ItemEntity(Level* level) : Entity(level) { _init(); }
	ItemEntity(Level* level, const Vec3& pos, const ItemInstance* itemInstance) : Entity(level) { _init(itemInstance, pos); }

	void burn(int damage) override;
	bool hurt(Entity* pCulprit, int damage) override;
	bool isInWater() override;
	void playerTouch(Player*) override;
	void tick() override;

	void checkInTile(const Vec3& pos);

public:
	// @NOTE: The original code keeps a pointer to something which is not duplicated with a new(), nor does it delete() the instance.
	// So either it's leaked, or the code will use invalid memory.
	// @NOTE: This is still a pointer in 0.2.1
#ifdef ORIGINAL_CODE
	ItemInstance* m_pItemInstance;
#else
	ItemInstance m_itemInstance;
#endif

	int field_E0;
	int field_E4;
	float field_E8;
	int field_EC;
	int m_health;
};

