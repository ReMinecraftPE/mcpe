/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Entity.hpp"

class FallingTile : public Entity
{
private:
	void _init(Level*, const Vec3& pos, int id);

public:
	FallingTile(Level*);
	FallingTile(Level*, const Vec3& pos, int id);

private:
	void _defineEntityData();

public:
	float getShadowHeightOffs() const override;
	bool isPickable() const override;
	void tick() override;
	void addAdditionalSaveData(CompoundTag& tag) const override;
	void readAdditionalSaveData(const CompoundTag& tag) override;

	int getTile() const;
	void setTile(int id);
	Level* getLevel() { return m_pLevel; }

public:
	int field_E0;
};

