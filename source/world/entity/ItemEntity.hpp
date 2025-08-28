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
	void _init(ItemInstance* itemInstance, const Vec3& pos);
public:
	ItemEntity(Level* level) : Entity(level) { _init(); }
	ItemEntity(Level* level, const Vec3& pos, ItemInstance* itemInstance) : Entity(level) { _init(itemInstance, pos); }
	~ItemEntity();

	void burn(int damage) override;
	bool hurt(Entity* pCulprit, int damage) override;
	bool isInWater() override;
	void playerTouch(Player*) override;
	void tick() override;
	void addAdditionalSaveData(CompoundTag& tag) const override;
	void readAdditionalSaveData(const CompoundTag& tag) override;

	void checkInTile(const Vec3& pos);

public:
	ItemInstance* m_pItemInstance;

	int m_age;
	int m_throwTime;
	float m_bobOffs;
	int m_tickCount;
	int m_health;
};

