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
	void _init(ItemInstance* itemInstance = nullptr);
	void _init(ItemInstance* itemInstance, float x, float y, float z);
public:
	ItemEntity(Level* level) : Entity(level) { _init(); }
	ItemEntity(Level* level, float x, float y, float z, ItemInstance* itemInstance) : Entity(level) { _init(itemInstance, x, y, z); }

	void burn(int damage) override;
	bool hurt(Entity* pCulprit, int damage) override;
	bool isInWater() override;
	void playerTouch(Player*) override;
	void tick() override;

	void checkInTile(float x, float y, float z);

public:
	// @NOTE: The original code keeps a pointer to something which is not duplicated with a new(), nor does it delete() the instance.
	// So either it's leaked, or the code will use invalid memory.
#ifdef ORIGINAL_CODE
	ItemInstance* m_pItemInstance;
#else
	ItemInstance m__itemInstance;
	ItemInstance* m_pItemInstance;
#endif

	int field_E0;
	int field_E4;
	float field_E8;
	int field_EC;
	int m_health;
};

